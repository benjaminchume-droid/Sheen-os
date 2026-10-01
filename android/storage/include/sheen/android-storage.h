#ifndef SHEEN_ANDROID_STORAGE_H
#define SHEEN_ANDROID_STORAGE_H
#include <stddef.h>
typedef enum {
    SHEEN_ANDROID_STORAGE_PRIVATE,
    SHEEN_ANDROID_STORAGE_CACHE,
    SHEEN_ANDROID_STORAGE_SHARED
} sheen_android_storage_kind;
typedef struct {
    char private_root[4096];
    char cache_root[4096];
    char shared_root[4096];
} sheen_android_storage;
int sheen_android_storage_init(sheen_android_storage *storage,
                               const char *private_root,
                               const char *cache_root,
                               const char *shared_root);
int sheen_android_storage_resolve(const sheen_android_storage *storage,
                                  sheen_android_storage_kind kind,
                                  const char *relative,
                                  char *out,size_t out_size);
int sheen_android_storage_mkdirs(const sheen_android_storage *storage,
                                 sheen_android_storage_kind kind,
                                 const char *relative);
#endif
