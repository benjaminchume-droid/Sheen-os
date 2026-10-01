#ifndef SHEEN_SHARPEN_H
#define SHEEN_SHARPEN_H
#include <stdint.h>
#include "sheen/vision-frame.h"
int sheen_sharpen_rgba(sheen_vision_frame *frame,uint8_t amount);
#endif
