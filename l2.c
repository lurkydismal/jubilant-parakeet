// hotreload_manager_specific.c
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <link.h>
#include <plthook.h>
#include <unistd.h>
#include <errno.h>

// --- Plugin definition ---

typedef struct {
    const char *path;       // original path as given (may be relative)
    char *canon_path;       // canonical absolute path via realpath
    time_t last_mtime;      // last seen modification time
    const char **symbols;   // array of symbol names to reload/patch
    size_t symbol_count;
} plugin_t;

typedef struct {
    char *name;
    void *addr;
} export_symbol_t;

// --- Patch context for a single plugin reload ---

typedef struct {
    const char *plugin_path;  // canonical path to match against info->dlpi_name
    const char **names;       // array of symbol names
    void **addrs;             // array of new addresses
    size_t count;
} patch_ctx_t;
// Collect all dynamic-exported functions from so_path on disk, resolve their addresses via dlsym(new_handle).
// Returns an allocated array of export_symbol_t (caller must free); sets *out_count.
// On error or no exports, returns NULL and *out_count=0.
static export_symbol_t *collect_exports(const char *so_path, void *new_handle, size_t *out_count) {
    *out_count = 0;
    if (!new_handle) {
        fprintf(stderr, "collect_exports: new_handle is NULL\n");
        return NULL;
    }
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "collect_exports: libelf initialization failed\n");
        return NULL;
    }
    int fd = open(so_path, O_RDONLY);
    if (fd < 0) {
        perror("collect_exports: open .so");
        return NULL;
    }
    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    if (!e) {
        fprintf(stderr, "collect_exports: elf_begin failed: %s\n", elf_errmsg(-1));
        close(fd);
        return NULL;
    }
    size_t shstrndx;
    if (elf_getshdrstrndx(e, &shstrndx) != 0) {
        fprintf(stderr, "collect_exports: elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        elf_end(e);
        close(fd);
        return NULL;
    }

    // First pass: find .dynsym section and count suitable symbols
    Elf_Scn *scn = NULL;
    Elf64_Shdr shdr;
    Elf_Data *data = NULL;
    size_t total = 0;
    while ((scn = elf_nextscn(e, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            continue;
        if (shdr.sh_type != SHT_DYNSYM)
            continue;
        data = elf_getdata(scn, NULL);
        if (!data) continue;
        size_t symcount = shdr.sh_size / shdr.sh_entsize;
        for (size_t i = 0; i < symcount; i++) {
            GElf_Sym sym;
            if (!gelf_getsym(data, i, &sym))
                continue;
            if (GELF_ST_TYPE(sym.st_info) == STT_FUNC && sym.st_shndx != SHN_UNDEF && sym.st_name != 0) {
                const char *name = elf_strptr(e, shdr.sh_link, sym.st_name);
                if (name && *name) {
                    total++;
                }
            }
        }
        break; // assume only one .dynsym
    }
    if (total == 0) {
        elf_end(e);
        close(fd);
        return NULL;
    }

    // Allocate array
    export_symbol_t *arr = calloc(total, sizeof(export_symbol_t));
    if (!arr) {
        perror("collect_exports: calloc");
        elf_end(e);
        close(fd);
        return NULL;
    }

    // Second pass: fill names and resolve addresses
    // Reset to beginning
    elf_end(e);
    lseek(fd, 0, SEEK_SET);
    e = elf_begin(fd, ELF_C_READ, NULL);
    if (!e) {
        fprintf(stderr, "collect_exports: elf_begin (2) failed: %s\n", elf_errmsg(-1));
        close(fd);
        free(arr);
        return NULL;
    }
    if (elf_getshdrstrndx(e, &shstrndx) != 0) {
        elf_end(e);
        close(fd);
        free(arr);
        return NULL;
    }
    scn = NULL;
    size_t idx = 0;
    while ((scn = elf_nextscn(e, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            continue;
        if (shdr.sh_type != SHT_DYNSYM)
            continue;
        data = elf_getdata(scn, NULL);
        if (!data) continue;
        size_t symcount = shdr.sh_size / shdr.sh_entsize;
        for (size_t i = 0; i < symcount; i++) {
            GElf_Sym sym;
            if (!gelf_getsym(data, i, &sym))
                continue;
            if (GELF_ST_TYPE(sym.st_info) == STT_FUNC && sym.st_shndx != SHN_UNDEF && sym.st_name != 0) {
                const char *name = elf_strptr(e, shdr.sh_link, sym.st_name);
                if (name && *name) {
                    // resolve via dlsym
                    dlerror();
                    void *addr = dlsym(new_handle, name);
                    const char *err = dlerror();
                    if (addr && !err) {
                        arr[idx].name = strdup(name);
                        arr[idx].addr = addr;
                        idx++;
                    } else {
                        // skip if not found
                    }
                }
            }
        }
        break;
    }
    elf_end(e);
    close(fd);
    *out_count = idx;
    if (idx == 0) {
        // no symbols resolved
        free(arr);
        return NULL;
    }
    return arr;
}

// Callback for dl_iterate_phdr: only patch if info->dlpi_name equals plugin_path
static int patch_phdr_callback(struct dl_phdr_info *info, size_t size, void *data) {
    patch_ctx_t *ctx = (patch_ctx_t *)data;
    const char *obj_path = info->dlpi_name;

    // Only patch if this module’s path matches the plugin_path exactly.
    // Skip everything else.
    if (!obj_path) {
        return 0;
    }
    // Compare strings: info->dlpi_name is typically absolute path.
    if (strcmp(obj_path, ctx->plugin_path) != 0) {
        return 0;
    }

    // Now obj_path == plugin_path: open its PLT/GOT for patching
    plthook_t *plthook = NULL;
    int open_ret = plthook_open(&plthook, obj_path);
    if (open_ret != 0) {
        // Could not open PLT for this object; skip
        fprintf(stderr, "[patch] plthook_open failed for %s: %s\n",
                obj_path, plthook_error());
        return 0;
    }

    unsigned int idx = 0;
    const char *symname;
    void **got_addr;
    while (plthook_enum(plthook, &idx, &symname, &got_addr) == 0) {
        for (size_t j = 0; j < ctx->count; j++) {
            if (strcmp(symname, ctx->names[j]) == 0) {
                *got_addr = ctx->addrs[j];
                printf("[patch] %s in %s -> %p\n", symname, obj_path, ctx->addrs[j]);
                break;
            }
        }
    }
    plthook_close(plthook);
    return 0;
}

// Reload one plugin if its file changed: dlopen, dlsym each symbol, then patch only that .so
// Returns 1 if reloaded & patched, 0 if not changed, -1 on error.
static int reload_plugin_if_needed(plugin_t *plug) {
    struct stat st;
    if (stat(plug->canon_path, &st) != 0) {
        fprintf(stderr, "[reload] stat(%s) failed: %s\n", plug->canon_path, strerror(errno));
        return -1;
    }
    if (st.st_mtime <= plug->last_mtime) {
        // No change
        return 0;
    }
    printf("[reload] Detected change in %s (old mtime=%ld, new mtime=%ld)\n",
           plug->canon_path, (long)plug->last_mtime, (long)st.st_mtime);

    // dlopen (or dlmopen) the plugin .so
    void *handle = NULL;
#if defined(LM_ID_NEWLM)
    dlerror();
    handle = dlmopen(LM_ID_NEWLM, plug->canon_path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "[reload] dlmopen(%s) failed: %s; falling back to dlopen\n",
                plug->canon_path, dlerror());
        dlerror();
        handle = dlopen(plug->canon_path, RTLD_NOW | RTLD_LOCAL);
    }
#else
    handle = dlopen(plug->canon_path, RTLD_NOW | RTLD_LOCAL);
#endif
    if (!handle) {
        fprintf(stderr, "[reload] dlopen(%s) failed: %s\n", plug->canon_path, dlerror());
        return -1;
    }

    // Resolve each symbol via dlsym
#if 0
    {
        void **addrs = malloc(sizeof(void*) * plug->symbol_count);
        if (!addrs) {
            perror("malloc");
            return -1;
        }
        for (size_t i = 0; i < plug->symbol_count; i++) {
            const char *name = plug->symbols[i];
            dlerror();
            void *addr = dlsym(handle, name);
            const char *err = dlerror();
            if (!addr || err) {
                fprintf(stderr, "[reload] dlsym(%s) failed: %s; skipping\n",
                        name, err?err:"<no error>");
                addrs[i] = NULL;
            } else {
                addrs[i] = addr;
                printf("[reload] dlsym(%s) -> %p\n", name, addr);
                ((void(*)(float))addr)( 123 );
            }
        }

        // Build patch context
        patch_ctx_t ctx = {
            .plugin_path = plug->canon_path,
            .names = plug->symbols,
            .addrs = addrs,
            .count = plug->symbol_count
        };
        // Patch only that plugin’s PLT
        dl_iterate_phdr(patch_phdr_callback, &ctx);

        plug->last_mtime = st.st_mtime;
        free(addrs);
    }
#endif

    {
        // After dlopen/dlmopen succeeded, collect all exported functions dynamically:
        size_t n_exports = 0;
        export_symbol_t *exports = collect_exports(plug->canon_path, handle, &n_exports);
        if (!exports || n_exports == 0) {
            fprintf(stderr, "[reload] No exported functions found or none resolved in %s\n", plug->canon_path);
            // Still update last_mtime so we don't keep retrying immediately
            plug->last_mtime = st.st_mtime;
            return 0; // or return 1? up to you: treat as reloaded but no exports
        }
        // Build arrays for patch context
        const char **names = malloc(sizeof(char*) * n_exports);
        void **addrs = malloc(sizeof(void*) * n_exports);
        if (!names || !addrs) {
            perror("malloc");
            // free exports
            for (size_t i = 0; i < n_exports; i++) free(exports[i].name);
            free(exports);
            free(names); free(addrs);
            return -1;
        }
        for (size_t i = 0; i < n_exports; i++) {
            names[i] = exports[i].name;
            addrs[i] = exports[i].addr;
            printf("[reload] export: %s -> %p\n", exports[i].name, exports[i].addr);
            ((void(*)(float))addrs[i])( 123 );
        }

        // Patch only that plugin’s PLT
        patch_ctx_t ctx = {
            .plugin_path = plug->canon_path,
            .names = names,
            .addrs = addrs,
            .count = n_exports
        };
        dl_iterate_phdr(patch_phdr_callback, &ctx);

        // Cleanup
        plug->last_mtime = st.st_mtime;
        // Free names from exports
        for (size_t i = 0; i < n_exports; i++) {
            free(exports[i].name);
        }
        free(exports);
        free(names);
        free(addrs);
    }

    // Do NOT dlclose(handle), so the new mapping stays valid for patched PLT entries
    return 1;
}

// Initialize plugin array: fill canon_path via realpath, set last_mtime=0
static void init_plugins(plugin_t *plugins, size_t plugin_count) {
    for (size_t i = 0; i < plugin_count; i++) {
        char buf[PATH_MAX];
        if (realpath(plugins[i].path, buf) != NULL) {
            plugins[i].canon_path = strdup(buf);
        } else {
            // If realpath fails, fall back to the given path
            fprintf(stderr, "[init] realpath(%s) failed: %s; using as-is\n",
                    plugins[i].path, strerror(errno));
            plugins[i].canon_path = strdup(plugins[i].path);
        }
        plugins[i].last_mtime = 0;
    }
}

// Free canonical paths if needed
static void free_plugins(plugin_t *plugins, size_t plugin_count) {
    for (size_t i = 0; i < plugin_count; i++) {
        free(plugins[i].canon_path);
    }
}

// Check all plugins, reload if needed
static void check_and_reload_all(plugin_t *plugins, size_t plugin_count) {
    for (size_t i = 0; i < plugin_count; i++) {
        int res = reload_plugin_if_needed(&plugins[i]);
        if (res > 0) {
            printf("[manager] Reloaded plugin: %s\n", plugins[i].canon_path);
        }
        // res==0: not changed; res<0: error printed
    }
}

// --- Example usage ---

// Suppose we have plugin paths and their symbol lists:
const char *plugin1_symbols[] = {
    "plugin_tick",
};

int main(int argc, char **argv) {
    // Example: override paths via argv if desired
    plugin_t plugins[] = {
        { .path = "./libplugin.so", .symbols = plugin1_symbols,
          .symbol_count = sizeof(plugin1_symbols)/sizeof(plugin1_symbols[0]) },
    };
    size_t plugin_count = sizeof(plugins)/sizeof(plugins[0]);

    init_plugins(plugins, plugin_count);

    printf("Plugin hot-reload manager started.\n");
    // Initial load
    check_and_reload_all(plugins, plugin_count);

    // Periodically check in a loop (or use inotify/signals etc.)
    while (1) {
        sleep(2);
        check_and_reload_all(plugins, plugin_count);
    }

    free_plugins(plugins, plugin_count);
    return 0;
}
