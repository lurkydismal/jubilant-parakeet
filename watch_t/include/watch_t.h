#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/inotify.h>

#define EVENT_WRITE IN_CLOSE_WRITE
#define EVENT_DELETE IN_DELETE
#define EVENT_RENAME ( IN_MOVED_FROM | IN_MOVED_TO )

#define DEFAULT_wATCH           \
    { .fileDescriptor = -1,     \
      .epollDescriptor = -1,    \
      .watchDescriptors = NULL, \
      .watchCallbacks = NULL,   \
      .callbackContexts = NULL }

typedef bool ( *watchCallback_t )( void* _context,
                                   const char* _fileName,
                                   size_t _eventsMask,
                                   uint32_t _cookie );

typedef struct {
    int fileDescriptor;
    int epollDescriptor;
    int* watchDescriptors;
    watchCallback_t* watchCallbacks;
    void** callbackContexts;
} watch_t;

watch_t watch_t$create( void );
bool watch_t$destroy( watch_t* _watch );

bool watch_t$add$toPath( watch_t* _watch,
                         const char* _path,
                         watchCallback_t _callback,
                         void* _context,
                         bool _isDirectory );
bool watch_t$add$toGlob( watch_t* _watch,
                         const char* _glob,
                         watchCallback_t _callback,
                         void* _context,
                         bool _onlyDirectories );
bool watch_t$remove( watch_t* _watch );

bool watch_t$check( watch_t* _watch, bool _isBlocking );
