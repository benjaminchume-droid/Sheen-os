#ifndef SHEEN_HDR_H
#define SHEEN_HDR_H
#include <stdint.h>
#include "sheen/vision-frame.h"
typedef enum {
    SHEEN_TRANSFER_SRGB,
    SHEEN_TRANSFER_PQ,
    SHEEN_TRANSFER_HLG
} sheen_transfer;
typedef enum {
    SHEEN_HDR_RANGE_FULL,
    SHEEN_HDR_RANGE_LIMITED
} sheen_hdr_range;
int sheen_hdr_p010_to_sdr_rgba(const sheen_vision_frame *src,sheen_vision_frame *dst,
                               sheen_transfer transfer,sheen_hdr_range range,
                               float target_nits);
#endif
