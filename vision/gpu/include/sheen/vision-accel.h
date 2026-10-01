#ifndef SHEEN_VISION_ACCEL_H
#define SHEEN_VISION_ACCEL_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_VISION_MAX_ACCEL 64
typedef struct {
    char device[128];
    uint64_t prime_cap;
    uint64_t addfb2_modifiers;
    uint64_t dumb_buffers;
    uint64_t syncobj;
    int accessible;
} sheen_vision_accel_device;
typedef struct {
    size_t count;
    sheen_vision_accel_device devices[SHEEN_VISION_MAX_ACCEL];
} sheen_vision_accel_inventory;
int sheen_vision_accel_probe(sheen_vision_accel_inventory *inventory);
#endif
