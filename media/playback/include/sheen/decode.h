#ifndef SHEEN_DECODE_H
#define SHEEN_DECODE_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_DECODE_QUEUE_CAPACITY 32
#define SHEEN_DECODE_PACKET_MAX 65536
typedef struct { uint8_t *data; size_t size; int64_t pts; int64_t dts; } sheen_packet;
typedef struct { uint8_t *data; size_t size; uint32_t width; uint32_t height; uint32_t format; int64_t pts; } sheen_frame;
typedef enum { SHEEN_DECODER_CLOSED, SHEEN_DECODER_OPEN, SHEEN_DECODER_DRAINING, SHEEN_DECODER_EOF, SHEEN_DECODER_ERROR } sheen_decoder_state;
typedef struct sheen_decoder_backend sheen_decoder_backend;
typedef struct sheen_decoder_pipeline sheen_decoder_pipeline;
sheen_decoder_pipeline *sheen_decode_create(const sheen_decoder_backend *backend);
void sheen_decode_destroy(sheen_decoder_pipeline *pipeline);
int sheen_decode_open(sheen_decoder_pipeline *pipeline,const char *codec_id,const void *config,size_t config_size);
int sheen_decode_send_packet(sheen_decoder_pipeline *pipeline,const uint8_t *data,size_t size,int64_t pts,int64_t dts);
int sheen_decode_receive_frame(sheen_decoder_pipeline *pipeline,sheen_frame *frame);
int sheen_decode_flush(sheen_decoder_pipeline *pipeline);
sheen_decoder_state sheen_decode_state(const sheen_decoder_pipeline *pipeline);
struct sheen_decoder_backend { int (*open)(void **ctx,const char *codec_id,const void *config,size_t config_size); int (*send)(void *ctx,const sheen_packet *packet); int (*receive)(void *ctx,sheen_frame *frame); int (*flush)(void *ctx); void (*close)(void *ctx); };
#endif
