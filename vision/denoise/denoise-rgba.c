#include <stdint.h>
#include "sheen/denoise.h"
static uint8_t avg3(const uint8_t *p0,const uint8_t *p1,const uint8_t *p2){return (uint8_t)(((unsigned)p0[0]+p1[0]+p2[0]+1)/3);}
int sheen_denoise_rgba(sheen_vision_frame *f,uint8_t strength){
    if(!f||strength>255)return 22;
    if(f->format!=SHEEN_PIXEL_RGBA8888&&f->format!=SHEEN_PIXEL_BGRA8888)return 95;
    if(sheen_vision_frame_validate(f))return 22;
    if(!strength)return 0;
    size_t bytes=(size_t)f->planes[0].stride*f->planes[0].height;uint8_t *tmp=malloc(bytes);if(!tmp)return 12;
    for(uint32_t y=0;y<f->height;y++)for(uint32_t x=0;x<f->width;x++){
        uint8_t *o=tmp+(size_t)y*f->planes[0].stride+(size_t)x*4;
        for(int k=0;k<3;k++){
            unsigned sum=0,count=0;
            for(int dy=-1;dy<=1;dy++){int yy=(int)y+dy;if(yy<0)yy=0;if(yy>=(int)f->height)yy=f->height-1;
                for(int dx=-1;dx<=1;dx++){int xx=(int)x+dx;if(xx<0)xx=0;if(xx>=(int)f->width)xx=f->width-1;
                    sum+=f->planes[0].data[(size_t)yy*f->planes[0].stride+(size_t)xx*4+k];count++;
                }}
            uint8_t filtered=(uint8_t)((sum+count/2)/count);
            uint8_t original=f->planes[0].data[(size_t)y*f->planes[0].stride+(size_t)x*4+k];
            o[k]=(uint8_t)(((unsigned)original*(255-strength)+(unsigned)filtered*strength+127)/255);
        }
        o[3]=f->planes[0].data[(size_t)y*f->planes[0].stride+(size_t)x*4+3];
    }
    memcpy(f->planes[0].data,tmp,bytes);free(tmp);return 0;
}
