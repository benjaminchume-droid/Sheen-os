#define _GNU_SOURCE
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/drm.h>
#include "sheen/android-graphics.h"

int sheen_android_graphics_validate(const sheen_android_buffer *b){
    if(!b||b->dma_buf_fd<0||!b->width||!b->height||!b->stride)return EINVAL;
    return 0;
}

int sheen_android_graphics_import(int drm_fd,sheen_android_buffer *b){
    if(drm_fd<0||sheen_android_graphics_validate(b))return EINVAL;
    if(b->drm_handle)return EALREADY;
    struct drm_prime_handle req={0};
    req.fd=b->dma_buf_fd;
    req.flags=DRM_CLOEXEC;
    if(ioctl(drm_fd,DRM_IOCTL_PRIME_FD_TO_HANDLE,&req)<0)return errno;
    b->drm_handle=req.handle;
    return 0;
}

int sheen_android_graphics_release(int drm_fd,sheen_android_buffer *b){
    if(drm_fd<0||!b)return EINVAL;
    if(!b->drm_handle)return 0;
    struct drm_gem_close req={0};
    req.handle=b->drm_handle;
    if(ioctl(drm_fd,DRM_IOCTL_GEM_CLOSE,&req)<0)return errno;
    b->drm_handle=0;
    return 0;
}
