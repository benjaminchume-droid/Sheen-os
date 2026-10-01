#ifndef SHEEN_MEDIA_LIBRARY_H
#define SHEEN_MEDIA_LIBRARY_H
#include <stddef.h>
#include <stdint.h>
typedef struct sheen_media_library sheen_media_library;
typedef struct { char id[65]; char source[4096]; char container[32]; uint64_t size_bytes; int64_t mtime_ns; } sheen_media_item;
sheen_media_library *sheen_media_library_open(const char *path);
void sheen_media_library_close(sheen_media_library *library);
int sheen_media_library_init(sheen_media_library *library);
int sheen_media_library_upsert(sheen_media_library *library,const sheen_media_item *item);
int sheen_media_library_scan_file(sheen_media_library *library,const char *path);
int sheen_media_library_scan_tree(sheen_media_library *library,const char *root);
int sheen_media_library_count(sheen_media_library *library,uint64_t *count);
int sheen_media_library_get_source(sheen_media_library *library,const char *id,sheen_media_item *item);
#endif
