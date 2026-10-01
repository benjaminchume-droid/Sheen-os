#ifndef SHEEN_STREAM_H
#define SHEEN_STREAM_H
#include <stddef.h>
#include <sys/types.h>
typedef enum { SHEEN_STREAM_FILE, SHEEN_STREAM_HTTP, SHEEN_STREAM_UDP } sheen_stream_type;
typedef struct sheen_stream sheen_stream;
sheen_stream *sheen_stream_open(const char *uri);
ssize_t sheen_stream_read(sheen_stream *stream,void *buffer,size_t capacity);
int sheen_stream_eof(const sheen_stream *stream);
void sheen_stream_close(sheen_stream *stream);
sheen_stream_type sheen_stream_type_get(const sheen_stream *stream);
const char *sheen_stream_uri(const sheen_stream *stream);
#endif
