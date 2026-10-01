#ifndef SHEEN_CAST_DISCOVERY_H
#define SHEEN_CAST_DISCOVERY_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_CAST_MAX_DEVICES 128
typedef struct { char device_id[256]; char name[256]; char address[64]; uint16_t port; char capabilities[512]; } sheen_cast_device;
typedef struct { size_t count; sheen_cast_device devices[SHEEN_CAST_MAX_DEVICES]; } sheen_cast_discovery_result;
int sheen_cast_discover(uint32_t timeout_ms,sheen_cast_discovery_result *result);
#endif
