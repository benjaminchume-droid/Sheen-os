#ifndef SHEEN_SCALER_H
#define SHEEN_SCALER_H
#include <stdint.h>
#include "sheen/vision-frame.h"
typedef enum { SHEEN_SCALE_NEAREST, SHEEN_SCALE_BILINEAR } sheen_scale_filter;
typedef struct sheen_scaler sheen_scaler;
sheen_scaler *sheen_scaler_create(uint32_t width,uint32_t height,sheen_scale_filter filter);
int sheen_scaler_process(sheen_scaler *scaler,const sheen_vision_frame *src,sheen_vision_frame *dst);
void sheen_scaler_close(sheen_scaler *scaler);
void sheen_vision_frame_free(sheen_vision_frame *frame);
#endif
