#ifndef SHEEN_ANDROID_GRAPHICS_H
#define SHEEN_ANDROID_GRAPHICS_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    int dma_buf_fd;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t drm_format;
    uint64_t modifier;
    uint32_t drm_handle;
} sheen_android_buffer;
int sheen_android_graphics_import(int drm_fd,sheen_android_buffer *buffer);
int sheen_android_graphics_release(int drm_fd,sheen_android_buffer *buffer);
int sheen_android_graphics_validate(const sheen_android_buffer *buffer);
#endif
