#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/superres.h"

struct sheen_superres { uint32_t scale; };

static uint8_t clamp8(int v){return (uint8_t)(v<0?0:v>255?255:v);}
static int alloc_output(const sheen_vision_frame *src,sheen_vision_frame *dst,uint32_t scale){
    memset(dst,0,sizeof(*dst));
    dst->width=src->width*scale;
    dst->height=src->height*scale;
    dst->format=src->format;
    dst->plane_count=1;
    size_t bytes=(size_t)dst->width*dst->height*4;
    dst->planes[0].data=malloc(bytes);
    if(!dst->planes[0].data)return ENOMEM;
    dst->planes[0].size=bytes;
    dst->planes[0].stride=dst->width*4;
    dst->planes[0].width=dst->width;
    dst->planes[0].height=dst->height;
    dst->pts=src->pts;dst->duration=src->duration;
    dst->color_primaries=src->color_primaries;
    dst->transfer=src->transfer;dst->matrix=src->matrix;dst->range=src->range;
    return 0;
}
static uint8_t pixel(const uint8_t *p){return *p;}
int sheen_superres_process(sheen_superres *sr,const sheen_vision_frame *src,sheen_vision_frame *dst){
    if(!sr||!src||!dst)return EINVAL;
    if(sr->scale<2||sr->scale>4)return EINVAL;
    if(src->format!=SHEEN_PIXEL_RGBA8888&&src->format!=SHEEN_PIXEL_BGRA8888)return ENOTSUP;
    if(sheen_vision_frame_validate(src))return EINVAL;
    int rc=alloc_output(src,dst,sr->scale);if(rc)return rc;
    for(uint32_t y=0;y<dst->height;y++){
        double fy=(double)y/(double)sr->scale;
        uint32_t y0=(uint32_t)fy; if(y0>=src->height)y0=src->height-1;
        uint32_t y1=y0+1<src->height?y0+1:y0;
        double wy=fy-(double)y0;
        for(uint32_t x=0;x<dst->width;x++){
            double fx=(double)x/(double)sr->scale;
            uint32_t x0=(uint32_t)fx; if(x0>=src->width)x0=src->width-1;
            uint32_t x1=x0+1<src->width?x0+1:x0;
            double wx=fx-(double)x0;
            const uint8_t *p00=src->planes[0].data+(size_t)y0*src->planes[0].stride+(size_t)x0*4;
            const uint8_t *p01=src->planes[0].data+(size_t)y0*src->planes[0].stride+(size_t)x1*4;
            const uint8_t *p10=src->planes[0].data+(size_t)y1*src->planes[0].stride+(size_t)x0*4;
            const uint8_t *p11=src->planes[0].data+(size_t)y1*src->planes[0].stride+(size_t)x1*4;
            uint8_t *o=dst->planes[0].data+(size_t)y*dst->planes[0].stride+(size_t)x*4;
            for(int k=0;k<3;k++){
                double a=p00[k]*(1-wx)+p01[k]*wx;
                double b=p10[k]*(1-wx)+p11[k]*wx;
                int base=(int)(a*(1-wy)+b*wy+0.5);
                int ix=(int)(fx+0.5);if(ix<0)ix=0;if(ix>=(int)src->width)ix=src->width-1;
                int iy=(int)(fy+0.5);if(iy<0)iy=0;if(iy>=(int)src->height)iy=src->height-1;
                const uint8_t *center=src->planes[0].data+(size_t)iy*src->planes[0].stride+(size_t)ix*4;
                const uint8_t *left=src->planes[0].data+(size_t)iy*src->planes[0].stride+(size_t)(ix?ix-1:ix)*4;
                const uint8_t *right=src->planes[0].data+(size_t)iy*src->planes[0].stride+(size_t)(ix+1<(int)src->width?ix+1:ix)*4;
                int residual=(int)pixel(center+k)-(int)((pixel(left+k)+pixel(right+k))/2);
                o[k]=clamp8(base+residual/2);
            }
            o[3]=p00[3];
        }
    }
    return 0;
}
sheen_superres *sheen_superres_create(uint32_t scale){if(scale<2||scale>4)return NULL;sheen_superres *s=calloc(1,sizeof(*s));if(s)s->scale=scale;return s;}
void sheen_superres_close(sheen_superres *s){free(s);}
