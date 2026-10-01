#ifndef SHEEN_HAL_H
#define SHEEN_HAL_H

#include <stddef.h>
#include <sys/types.h>

#define SHEEN_HAL_PATH_MAX 4096

typedef enum {
    SHEEN_DEVICE_PRESENT = 0,
    SHEEN_DEVICE_INITIALIZING,
    SHEEN_DEVICE_READY,
    SHEEN_DEVICE_DEGRADED,
    SHEEN_DEVICE_FAILED,
    SHEEN_DEVICE_REMOVED
} sheen_device_state;

typedef struct {
    char device_id[SHEEN_HAL_PATH_MAX];
    char class_name[64];
    char name[256];
    char sysfs_path[SHEEN_HAL_PATH_MAX];
    sheen_device_state state;
} sheen_hal_device;

int sheen_hal_sysfs_available(void);
int sheen_hal_read_text(const char *path, char *buf, size_t len);
ssize_t sheen_hal_read_link(const char *path, char *buf, size_t len);
int sheen_hal_join_path(char *out, size_t len, const char *base, const char *name);
int sheen_hal_populate_device(sheen_hal_device *device, const char *class_name, const char *name, const char *path);
const char *sheen_hal_state_name(sheen_device_state state);

#endif
