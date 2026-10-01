#ifndef SHEEN_DEBLOCK_H
#define SHEEN_DEBLOCK_H
#include <stdint.h>
#include "sheen/vision-frame.h"
int sheen_deblock_rgba(sheen_vision_frame *frame,uint32_t block_size,uint8_t strength);
#endif
