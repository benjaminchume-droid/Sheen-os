#ifndef SHEEN_SUPERRES_H
#define SHEEN_SUPERRES_H
#include <stdint.h>
#include "sheen/vision-frame.h"
typedef struct sheen_superres sheen_superres;
sheen_superres *sheen_superres_create(uint32_t scale);
int sheen_superres_process(sheen_superres *sr,const sheen_vision_frame *src,sheen_vision_frame *dst);
void sheen_superres_close(sheen_superres *sr);
#endif
