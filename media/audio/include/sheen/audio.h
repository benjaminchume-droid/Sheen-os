#ifndef SHEEN_AUDIO_H
#define SHEEN_AUDIO_H
#include <stddef.h>
#include <stdint.h>
typedef enum { SHEEN_AUDIO_S16LE=1, SHEEN_AUDIO_S32LE=2, SHEEN_AUDIO_F32LE=3 } sheen_audio_sample_format;
typedef struct { uint32_t sample_rate; uint16_t channels; sheen_audio_sample_format format; } sheen_audio_format;
typedef struct { const uint8_t *data; size_t bytes; uint64_t frames; int64_t pts; sheen_audio_format format; } sheen_audio_buffer;
typedef enum { SHEEN_AUDIO_CLOSED, SHEEN_AUDIO_OPEN, SHEEN_AUDIO_DRAINING, SHEEN_AUDIO_EOF, SHEEN_AUDIO_ERROR } sheen_audio_state;
typedef struct sheen_audio_sink sheen_audio_sink;
typedef struct sheen_audio_pipeline sheen_audio_pipeline;
sheen_audio_pipeline *sheen_audio_create(const sheen_audio_sink *sink);
void sheen_audio_destroy(sheen_audio_pipeline *pipeline);
int sheen_audio_open(sheen_audio_pipeline *pipeline,const sheen_audio_format *format);
int sheen_audio_write(sheen_audio_pipeline *pipeline,const uint8_t *data,size_t bytes,uint64_t frames,int64_t pts);
int sheen_audio_flush(sheen_audio_pipeline *pipeline);
sheen_audio_state sheen_audio_state_get(const sheen_audio_pipeline *pipeline);
struct sheen_audio_sink { int (*open)(void **ctx,const sheen_audio_format *format); int (*write)(void *ctx,const sheen_audio_buffer *buffer); int (*flush)(void *ctx); void (*close)(void *ctx); };
#endif
