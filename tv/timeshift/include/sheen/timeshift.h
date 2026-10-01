#ifndef SHEEN_TIMESHIFT_H
#define SHEEN_TIMESHIFT_H
#include <stddef.h>
#include <stdint.h>
typedef struct sheen_timeshift sheen_timeshift;
typedef struct { uint64_t oldest_ms; uint64_t newest_ms; uint64_t retained_bytes; } sheen_timeshift_info;
sheen_timeshift *sheen_timeshift_open(const char *directory,uint64_t max_bytes,uint64_t segment_bytes);
int sheen_timeshift_append(sheen_timeshift *buffer,const void *data,size_t bytes,uint64_t pts_ms);
int sheen_timeshift_info_get(sheen_timeshift *buffer,sheen_timeshift_info *info);
int sheen_timeshift_seek(sheen_timeshift *buffer,uint64_t position_ms);
ssize_t sheen_timeshift_read(sheen_timeshift *buffer,void *data,size_t capacity,uint64_t *pts_ms);
int sheen_timeshift_at_live_edge(sheen_timeshift *buffer);
void sheen_timeshift_close(sheen_timeshift *buffer);
#endif
