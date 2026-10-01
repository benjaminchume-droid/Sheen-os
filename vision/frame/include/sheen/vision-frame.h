#ifndef SHEEN_VISION_FRAME_H
#define SHEEN_VISION_FRAME_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_VISION_MAX_PLANES 4
#define SHEEN_VISION_MAX_STAGES 32
typedef enum {
    SHEEN_PIXEL_UNKNOWN,
    SHEEN_PIXEL_NV12,
    SHEEN_PIXEL_YUV420P,
    SHEEN_PIXEL_P010,
    SHEEN_PIXEL_RGBA8888,
    SHEEN_PIXEL_BGRA8888
} sheen_pixel_format;
typedef struct {
    uint8_t *data;
    size_t size;
    uint32_t stride;
    uint32_t width;
    uint32_t height;
} sheen_frame_plane;
typedef struct {
    uint32_t width;
    uint32_t height;
    sheen_pixel_format format;
    uint8_t plane_count;
    sheen_frame_plane planes[SHEEN_VISION_MAX_PLANES];
    int64_t pts;
    int64_t duration;
    uint32_t color_primaries;
    uint32_t transfer;
    uint32_t matrix;
    uint32_t range;
} sheen_vision_frame;
typedef struct sheen_vision_stage sheen_vision_stage;
typedef struct sheen_vision_pipeline sheen_vision_pipeline;
struct sheen_vision_stage {
    const char *id;
    int (*process)(void *ctx,sheen_vision_frame *frame);
    void (*close)(void *ctx);
    void *ctx;
};
sheen_vision_pipeline *sheen_vision_pipeline_create(void);
int sheen_vision_pipeline_add(sheen_vision_pipeline *pipeline,const sheen_vision_stage *stage);
int sheen_vision_pipeline_process(sheen_vision_pipeline *pipeline,sheen_vision_frame *frame);
void sheen_vision_pipeline_close(sheen_vision_pipeline *pipeline);
int sheen_vision_frame_validate(const sheen_vision_frame *frame);
void sheen_vision_frame_release(sheen_vision_frame *frame);
#endif
