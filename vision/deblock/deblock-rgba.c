#include <stdint.h>
#include <stdlib.h>
#include "sheen/deblock.h"
static uint8_t clamp8(int v){return (uint8_t)(v<0?0:v>255?255:v);}
int sheen_deblock_rgba(sheen_vision_frame *f,uint32_t block,uint8_t strength){
    if(!f||!block||strength>255)return 22;
    if(f->format!=SHEEN_PIXEL_RGBA8888&&f->format!=SHEEN_PIXEL_BGRA8888)return 95;
    if(sheen_vision_frame_validate(f))return 22;
    if(!strength)return 0;
    uint8_t *tmp=malloc((size_t)f->width*f->height*4);if(!tmp)return 12;
    memcpy(tmp,f->planes[0].data,(size_t)f->width*f->height*4);
    for(uint32_t y=0;y<f->height;y++)for(uint32_t x=0;x<f->width;x++){
        if(x==0||y==0)continue;
        int edge_x=(x%block)==0,edge_y=(y%block)==0;if(!edge_x&&!edge_y)continue;
        size_t a=((size_t)y*f->planes[0].stride+(size_t)x*4), l=((size_t)y*f->planes[0].stride+(size_t)(x-1)*4), u=((size_t)(y-1)*f->planes[0].stride+(size_t)x*4);
        for(int k=0;k<3;k++){
            int base=f->planes[0].data[a+k];int ref=edge_x?f->planes[0].data[l+k]:f->planes[0].data[u+k];
            int blended=base+(ref-base)*(int)strength/255;
            tmp[(size_t)y*f->planes[0].stride+(size_t)x*4+k]=clamp8(blended);
        }
    }
    memcpy(f->planes[0].data,tmp,(size_t)f->width*f->height*4);free(tmp);return 0;
}
