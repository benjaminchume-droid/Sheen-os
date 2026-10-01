#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/hdr.h"

static uint8_t clamp8(float v){
    if(v<0.0f)v=0.0f;
    if(v>1.0f)v=1.0f;
    return (uint8_t)(v*255.0f+0.5f);
}
static float pq_eotf(float n){
    const float m1=2610.0f/16384.0f;
    const float m2=2523.0f/32.0f;
    const float c1=3424.0f/4096.0f;
    const float c2=2413.0f/128.0f;
    const float c3=2392.0f/128.0f;
    float p=powf(fmaxf(n,0.0f),1.0f/m2);
    float num=fmaxf(p-c1,0.0f);
    float den=c2-c3*p;
    if(den<=0.0f)return 0.0f;
    return 10000.0f*powf(num/den,1.0f/m1);
}
static float srgb_oetf(float l){
    l=fmaxf(l,0.0f);
    return l<=0.0031308f?12.92f*l:1.055f*powf(l,1.0f/2.4f)-0.055f;
}
static int alloc_rgba(const sheen_vision_frame *s,sheen_vision_frame *d){
    memset(d,0,sizeof(*d));d->width=s->width;d->height=s->height;d->format=SHEEN_PIXEL_RGBA8888;d->plane_count=1;
    size_t bytes=(size_t)d->width*d->height*4;d->planes[0].data=malloc(bytes);if(!d->planes[0].data)return ENOMEM;
    d->planes[0].size=bytes;d->planes[0].stride=d->width*4;d->planes[0].width=d->width;d->planes[0].height=d->height;
    d->pts=s->pts;d->duration=s->duration;d->color_primaries=s->color_primaries;d->transfer=1;d->matrix=s->matrix;d->range=1;
    return 0;
}
static uint16_t p010_sample(const uint8_t *p){
    uint16_t v=(uint16_t)p[0]|((uint16_t)p[1]<<8);
    return (uint16_t)(v>>6);
}
int sheen_hdr_p010_to_sdr_rgba(const sheen_vision_frame *src,sheen_vision_frame *dst,
                               sheen_transfer transfer,sheen_hdr_range range,float target_nits){
    if(!src||!dst||target_nits<=0.0f)return EINVAL;
    if(src->format!=SHEEN_PIXEL_P010||src->plane_count<2)return EINVAL;
    if(sheen_vision_frame_validate(src))return EINVAL;
    if(transfer!=SHEEN_TRANSFER_PQ)return ENOTSUP;
    int rc=alloc_rgba(src,dst);if(rc)return rc;
    float inv_y=range==SHEEN_HDR_RANGE_LIMITED?1.0f/876.0f:1.0f/1023.0f;
    float inv_c=range==SHEEN_HDR_RANGE_LIMITED?1.0f/896.0f:1.0f/1023.0f;
    float y_bias=range==SHEEN_HDR_RANGE_LIMITED?64.0f:0.0f;
    float c_bias=range==SHEEN_HDR_RANGE_LIMITED?512.0f:511.5f;

    for(uint32_t y=0;y<src->height;y++){
        uint8_t *out=dst->planes[0].data+(size_t)y*dst->planes[0].stride;
        const uint8_t *yr=src->planes[0].data+(size_t)y*src->planes[0].stride;
        const uint8_t *uvr=src->planes[1].data+(size_t)(y/2)*src->planes[1].stride;
        for(uint32_t x=0;x<src->width;x++){
            uint32_t cx=(x/2)*2;
            float code_y=(float)p010_sample(yr+(size_t)x*2);
            float code_u=(float)p010_sample(uvr+(size_t)cx*2);
            float code_v=(float)p010_sample(uvr+(size_t)(cx+1)*2);
            float yv=(code_y-y_bias)*inv_y;
            float u=(code_u-c_bias)*inv_c;
            float v=(code_v-c_bias)*inv_c;
            yv=fmaxf(0.0f,yv);
            float r=yv+1.4746f*v;
            float g=yv-0.16455f*u-0.57135f*v;
            float b=yv+1.8814f*u;
            float values[3]={r,g,b};
            for(int k=0;k<3;k++){
                float n=values[k];
                float nits=pq_eotf(fmaxf(n,0.0f));
                float norm=nits/fmaxf(target_nits,1.0f);
                float mapped=norm/(1.0f+norm);
                values[k]=srgb_oetf(mapped);
            }
            uint8_t *p=out+(size_t)x*4;
            p[0]=clamp8(values[0]);p[1]=clamp8(values[1]);p[2]=clamp8(values[2]);p[3]=255;
        }
    }
    return 0;
}
