#ifndef SHEEN_DENOISE_H
#define SHEEN_DENOISE_H
#include <stdint.h>
#include "sheen/vision-frame.h"
int sheen_denoise_rgba(sheen_vision_frame *frame,uint8_t strength);
#endif
