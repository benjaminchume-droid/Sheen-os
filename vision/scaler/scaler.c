#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/scaler.h"

struct sheen_scaler { uint32_t width,height; sheen_scale_filter filter; };

sheen_scaler *sheen_scaler_create(uint32_t w,uint32_t h,sheen_scale_filter f){
    if(!w||!h)return NULL;
    sheen_scaler *s=calloc(1,sizeof(*s));
    if(!s)return NULL;
    s->width=w;s->height=h;s->filter=f;
    return s;
}

static int alloc_frame(sheen_vision_frame *f,sheen_pixel_format format,uint32_t w,uint32_t h){
    memset(f,0,sizeof(*f));
    f->width=w;f->height=h;f->format=format;
    if(format==SHEEN_PIXEL_RGBA8888||format==SHEEN_PIXEL_BGRA8888){
        size_t bytes=(size_t)w*h*4;
        f->plane_count=1;
        f->planes[0].data=malloc(bytes);
        if(!f->planes[0].data)return ENOMEM;
        f->planes[0].size=bytes;f->planes[0].stride=w*4;f->planes[0].width=w;f->planes[0].height=h;
    } else if(format==SHEEN_PIXEL_NV12||format==SHEEN_PIXEL_P010){
        size_t bpp=format==SHEEN_PIXEL_P010?2:1;
        size_t y=(size_t)w*h*bpp;
        size_t uv=(size_t)w*((h+1)/2)*bpp;
        f->plane_count=2;
        f->planes[0].data=malloc(y);
        f->planes[1].data=malloc(uv);
        if(!f->planes[0].data||!f->planes[1].data){sheen_vision_frame_free(f);return ENOMEM;}
        f->planes[0].size=y;f->planes[0].stride=w*bpp;f->planes[0].width=w;f->planes[0].height=h;
        f->planes[1].size=uv;f->planes[1].stride=w*bpp;f->planes[1].width=w;f->planes[1].height=(h+1)/2;
    } else return ENOTSUP;
    return 0;
}

void sheen_vision_frame_free(sheen_vision_frame *f){
    if(!f)return;
    for(size_t i=0;i<f->plane_count;i++){free(f->planes[i].data);f->planes[i].data=NULL;}
    f->plane_count=0;
}

static uint32_t map_coord(uint32_t out,uint32_t in,uint32_t pos){
    return (uint32_t)(((uint64_t)pos*in)/out);
}

int sheen_scaler_process(sheen_scaler *s,const sheen_vision_frame *src,sheen_vision_frame *dst){
    if(!s||!src||!dst)return EINVAL;
    int rc=sheen_vision_frame_validate(src);if(rc)return rc;
    if(src->format!=SHEEN_PIXEL_RGBA8888&&src->format!=SHEEN_PIXEL_BGRA8888&&
       src->format!=SHEEN_PIXEL_NV12&&src->format!=SHEEN_PIXEL_P010)return ENOTSUP;
    rc=alloc_frame(dst,src->format,s->width,s->height);if(rc)return rc;
    dst->pts=src->pts;dst->duration=src->duration;dst->color_primaries=src->color_primaries;
    dst->transfer=src->transfer;dst->matrix=src->matrix;dst->range=src->range;

    if(src->format==SHEEN_PIXEL_RGBA8888||src->format==SHEEN_PIXEL_BGRA8888){
        for(uint32_t y=0;y<s->height;y++){
            uint32_t sy=map_coord(s->height,src->height,y);
            const uint8_t *row=src->planes[0].data+(size_t)sy*src->planes[0].stride;
            uint8_t *out=dst->planes[0].data+(size_t)y*dst->planes[0].stride;
            for(uint32_t x=0;x<s->width;x++){
                uint32_t sx=map_coord(s->width,src->width,x);
                memcpy(out+(size_t)x*4,row+(size_t)sx*4,4);
            }
        }
        return 0;
    }

    uint32_t sy_h=(src->height+1)/2, dy_h=(s->height+1)/2;
    size_t bpp=src->format==SHEEN_PIXEL_P010?2:1;
    for(uint32_t y=0;y<s->height;y++){
        uint32_t sy=map_coord(s->height,src->height,y);
        for(uint32_t x=0;x<s->width;x++){
            uint32_t sx=map_coord(s->width,src->width,x);
            memcpy(dst->planes[0].data+(size_t)y*dst->planes[0].stride+(size_t)x*bpp,
                   src->planes[0].data+(size_t)sy*src->planes[0].stride+(size_t)sx*bpp,bpp);
        }
    }
    for(uint32_t y=0;y<dy_h;y++){
        uint32_t sy=map_coord(dy_h,sy_h,y);
        for(uint32_t x=0;x<s->width;x+=2){
            uint32_t sx=map_coord(s->width,src->width,x);
            size_t count=(x+1<s->width)?2:1;
            memcpy(dst->planes[1].data+(size_t)y*dst->planes[1].stride+(size_t)x*bpp,
                   src->planes[1].data+(size_t)sy*src->planes[1].stride+(size_t)sx*bpp,count*bpp);
        }
    }
    return 0;
}

void sheen_scaler_close(sheen_scaler *s){free(s);}
