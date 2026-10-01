#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/vision-frame.h"

struct sheen_vision_pipeline {
    sheen_vision_stage stages[SHEEN_VISION_MAX_STAGES];
    size_t count;
};

static int plane_required(sheen_pixel_format f,size_t plane) {
    switch(f) {
        case SHEEN_PIXEL_NV12: return plane < 2;
        case SHEEN_PIXEL_YUV420P: return plane < 3;
        case SHEEN_PIXEL_P010: return plane < 2;
        case SHEEN_PIXEL_RGBA8888:
        case SHEEN_PIXEL_BGRA8888: return plane < 1;
        default: return 0;
    }
}
int sheen_vision_frame_validate(const sheen_vision_frame *f) {
    if(!f || !f->width || !f->height || f->plane_count > SHEEN_VISION_MAX_PLANES)
        return EINVAL;
    for(size_t i=0;i<f->plane_count;i++) {
        if(!plane_required(f->format,i)) return EINVAL;
        if(!f->planes[i].data || !f->planes[i].size || !f->planes[i].stride)
            return EINVAL;
        size_t min_height=f->height;
        if((f->format==SHEEN_PIXEL_NV12||f->format==SHEEN_PIXEL_P010) && i==1) min_height=(f->height+1)/2;
        uint32_t min_stride=f->width;
        if(f->format==SHEEN_PIXEL_P010 || f->format==SHEEN_PIXEL_YUV420P && i>0) min_stride=f->width*2;
        if(f->format==SHEEN_PIXEL_RGBA8888 || f->format==SHEEN_PIXEL_BGRA8888) min_stride=f->width*4;
        if(f->planes[i].stride < min_stride) return EINVAL;
        if((size_t)f->planes[i].stride * min_height > f->planes[i].size) return EINVAL;
    }
    return 0;
}
sheen_vision_pipeline *sheen_vision_pipeline_create(void) {
    return calloc(1,sizeof(sheen_vision_pipeline));
}
int sheen_vision_pipeline_add(sheen_vision_pipeline *p,const sheen_vision_stage *s) {
    if(!p||!s||!s->id||!s->process||p->count>=SHEEN_VISION_MAX_STAGES) return EINVAL;
    p->stages[p->count++]=*s;
    return 0;
}
int sheen_vision_pipeline_process(sheen_vision_pipeline *p,sheen_vision_frame *f) {
    if(!p||!f) return EINVAL;
    int rc=sheen_vision_frame_validate(f);
    if(rc) return rc;
    for(size_t i=0;i<p->count;i++) {
        rc=p->stages[i].process(p->stages[i].ctx,f);
        if(rc) return rc;
        rc=sheen_vision_frame_validate(f);
        if(rc) return rc;
    }
    return 0;
}
void sheen_vision_pipeline_close(sheen_vision_pipeline *p) {
    if(!p)return;
    for(size_t i=0;i<p->count;i++) if(p->stages[i].close) p->stages[i].close(p->stages[i].ctx);
    free(p);
}

void sheen_vision_frame_release(sheen_vision_frame *f){if(!f)return;for(size_t i=0;i<f->plane_count;i++){free(f->planes[i].data);f->planes[i].data=NULL;}memset(f,0,sizeof(*f));}
