#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plthook.h>

// Signature of plugin function(s):
typedef void (*plugin_tick_t)(float);

// Global to hold new pointers:
static plugin_tick_t new_plugin_tick = NULL;

// List of symbol names we patch:
static const char *symbols_to_patch[] = {
    "plugin_tick",
    NULL
};

static void *load_new_plugin(const char *path) {
    void *handle = dlopen(path, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "dlopen(%s) failed: %s\n", path, dlerror());
        return NULL;
    }
    // Option A: Direct dlsym per known symbol
    plugin_tick_t f = (plugin_tick_t)dlsym(handle, "plugin_tick");
    if (!f) {
        fprintf(stderr, "Warning: dlsym(plugin_tick) failed: %s\n", dlerror());
    } else {
        new_plugin_tick = f;
    }
    // Option B: If plugin exports a symbol table, call that instead
    return handle;
}

static int patch_symbol_in_module(const char *module_path_or_NULL, const char *symbol_name, void *new_func) {
    plthook_t *plthook = NULL;
    int ret;
    if (module_path_or_NULL == NULL) {
        ret = plthook_open(&plthook, NULL);  // main executable
    } else {
        ret = plthook_open(&plthook, module_path_or_NULL);
    }
    if (ret != 0) {
        fprintf(stderr, "plthook_open(%s) failed: %s\n",
                module_path_or_NULL?module_path_or_NULL:"<main>", plthook_error());
        return -1;
    }
    if (plthook_replace(plthook, symbol_name, new_func, NULL) != 0) {
        fprintf(stderr, "plthook_replace(%s) failed: %s\n", symbol_name, plthook_error());
        plthook_close(plthook);
        return -1;
    }
    plthook_close(plthook);
    return 0;
}

void reload_plugin(const char *new_so_path) {
    void *handle = load_new_plugin(new_so_path);
    if (!handle) return;

    // TODO: synchronize threads: ensure no thread is in plugin function.
    // e.g., pause others, or set a flag so they don’t enter plugin code.

    // Patch main executable:
    for (const char **p = symbols_to_patch; *p; ++p) {
        const char *sym = *p;
        void *newf = NULL;
        if (strcmp(sym, "plugin_tick") == 0 && new_plugin_tick) {
            newf = (void*)new_plugin_tick;
        }
        if (newf) {
            if (patch_symbol_in_module(NULL, sym, newf) == 0) {
                fprintf(stderr, "Patched main: %s -> %p\n", sym, newf);
            }
        } else {
            fprintf(stderr, "No new function pointer for %s\n", sym);
        }
    }

    // If other modules also call plugin_tick, patch them too:
    // You could iterate with dl_iterate_phdr() and for each mapped SO open with plthook.
    // For brevity, omitted here.

    // Optionally dlclose(handle) when you no longer need the extra refcount.
    // But the mapping remains while original DT_NEEDED reference exists.
    // dlclose(handle);
}

// Example of calling plugin_tick in main code:
extern void plugin_tick(float);  // originally linked, but after patch, calls go to new_plugin_tick
int main(int argc, char **argv) {
    float dt = 0.016f;
    printf("Before reload: calling plugin_tick...\n");
    plugin_tick(dt);  // goes to old SO’s implementation

    // Hot-reload:
    reload_plugin("libplugin_new.so");

    printf("After reload: calling plugin_tick...\n");
    plugin_tick(dt);  // now goes to new implementation

    return 0;
}
