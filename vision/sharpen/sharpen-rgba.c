#include <stdint.h>
#include <stdlib.h>
#include "sheen/sharpen.h"
static uint8_t clamp8(int v){return (uint8_t)(v<0?0:v>255?255:v);}
int sheen_sharpen_rgba(sheen_vision_frame *f,uint8_t amount){
    if(!f||amount>255)return 22;
    if(f->format!=SHEEN_PIXEL_RGBA8888&&f->format!=SHEEN_PIXEL_BGRA8888)return 95;
    if(sheen_vision_frame_validate(f))return 22;
    if(!amount)return 0;
    size_t bytes=(size_t)f->width*f->height*4;uint8_t *tmp=malloc(bytes);if(!tmp)return 12;
    memcpy(tmp,f->planes[0].data,bytes);
    for(uint32_t y=1;y+1<f->height;y++)for(uint32_t x=1;x+1<f->width;x++){
        size_t i=(size_t)y*f->planes[0].stride+(size_t)x*4;
        for(int k=0;k<3;k++){
            int c=f->planes[0].data[i+k];int n=((int)f->planes[0].data[i-4+k]+f->planes[0].data[i+4+k]+f->planes[0].data[i-f->planes[0].stride+k]+f->planes[0].data[i+f->planes[0].stride+k])/4;
            tmp[i+k]=clamp8(c+(c-n)*(int)amount/255);
        }
    }
    memcpy(f->planes[0].data,tmp,bytes);free(tmp);return 0;
}
