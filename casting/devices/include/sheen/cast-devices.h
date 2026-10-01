#ifndef SHEEN_CAST_DEVICES_H
#define SHEEN_CAST_DEVICES_H
#include <stddef.h>
#include <stdint.h>
typedef enum { SHEEN_CAST_DEVICE_PRESENT, SHEEN_CAST_DEVICE_STALE, SHEEN_CAST_DEVICE_REMOVED } sheen_cast_device_state;
typedef struct {
    char device_id[256];
    char name[256];
    char address[64];
    uint16_t port;
    char capabilities[512];
    uint64_t last_seen_ms;
    sheen_cast_device_state state;
} sheen_cast_managed_device;
typedef struct sheen_cast_devices sheen_cast_devices;
sheen_cast_devices *sheen_cast_devices_open(const char *path);
int sheen_cast_devices_init(sheen_cast_devices *db);
int sheen_cast_devices_upsert(sheen_cast_devices *db,const sheen_cast_managed_device *device);
int sheen_cast_devices_mark_missing(sheen_cast_devices *db,const char *device_id);
int sheen_cast_devices_get(sheen_cast_devices *db,const char *device_id,sheen_cast_managed_device *device);
int sheen_cast_devices_count(sheen_cast_devices *db,uint64_t *count);
void sheen_cast_devices_close(sheen_cast_devices *db);
const char *sheen_cast_device_state_name(sheen_cast_device_state state);
#endif
