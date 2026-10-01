#ifndef SHEEN_CONTAINER_H
#define SHEEN_CONTAINER_H
#include <stddef.h>
#include <stdint.h>
typedef enum { SHEEN_CONTAINER_UNKNOWN=0, SHEEN_CONTAINER_ISOBMFF, SHEEN_CONTAINER_MATROSKA, SHEEN_CONTAINER_MPEG_TS, SHEEN_CONTAINER_OGG, SHEEN_CONTAINER_FLV, SHEEN_CONTAINER_RIFF, SHEEN_CONTAINER_MPEG_PS, SHEEN_CONTAINER_ASF } sheen_container_type;
typedef struct { sheen_container_type type; char name[32]; char mime[64]; char source[4096]; uint64_t size_bytes; size_t bytes_inspected; int seekable; } sheen_container_info;
int sheen_container_inspect_file(const char *path,sheen_container_info *info);
const char *sheen_container_type_name(sheen_container_type type);
const char *sheen_container_mime(sheen_container_type type);
#endif
