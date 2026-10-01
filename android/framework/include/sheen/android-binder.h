#ifndef SHEEN_ANDROID_BINDER_H
#define SHEEN_ANDROID_BINDER_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    char path[256];
    int available;
    uint32_t protocol_version;
    uint64_t buffer_size;
} sheen_android_binder_info;
int sheen_android_binder_probe(sheen_android_binder_info *info);
int sheen_android_binder_open(const sheen_android_binder_info *info);
int sheen_android_binder_query_version(int fd,uint32_t *version);
int sheen_android_binder_mountfs(const char *mountpoint);
#endif
