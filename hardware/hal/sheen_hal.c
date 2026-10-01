#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sheen/hal.h"

int sheen_hal_sysfs_available(void) { return access("/sys", R_OK | X_OK) == 0; }

int sheen_hal_read_text(const char *path, char *buf, size_t len) {
    if (!path || !buf || len == 0) return -1;
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    int ok = fgets(buf, (int)len, f) ? 0 : -1;
    fclose(f);
    if (ok == 0) buf[strcspn(buf, "\n")] = 0;
    return ok;
}

ssize_t sheen_hal_read_link(const char *path, char *buf, size_t len) {
    if (!path || !buf || len < 2) return -1;
    ssize_t n = readlink(path, buf, len - 1);
    if (n < 0) return -1;
    buf[n] = 0;
    return n;
}

int sheen_hal_join_path(char *out, size_t len, const char *base, const char *name) {
    if (!out || !base || !name || len == 0) return -1;
    size_t a = strlen(base), b = strlen(name);
    int slash = a > 0 && base[a - 1] != '/';
    if (a + (size_t)slash + b + 1 > len) return -1;
    memcpy(out, base, a);
    size_t p = a;
    if (slash) out[p++] = '/';
    memcpy(out + p, name, b);
    out[p + b] = 0;
    return 0;
}

static void copy_field(char *dst, size_t dst_len, const char *src) {
    if (!src || dst_len == 0) return;
    snprintf(dst, dst_len, "%s", src);
}

int sheen_hal_populate_device(sheen_hal_device *d, const char *class_name, const char *name, const char *path) {
    if (!d || !class_name || !name || !path) return -1;
    memset(d, 0, sizeof(*d));
    copy_field(d->class_name, sizeof(d->class_name), class_name);
    copy_field(d->name, sizeof(d->name), name);
    copy_field(d->sysfs_path, sizeof(d->sysfs_path), path);
    d->state = SHEEN_DEVICE_PRESENT;
    char real[SHEEN_HAL_PATH_MAX];
    if (sheen_hal_read_link(path, real, sizeof(real)) >= 0) copy_field(d->device_id, sizeof(d->device_id), real);
    else copy_field(d->device_id, sizeof(d->device_id), path);
    return 0;
}

const char *sheen_hal_state_name(sheen_device_state state) {
    switch (state) {
        case SHEEN_DEVICE_PRESENT: return "present";
        case SHEEN_DEVICE_INITIALIZING: return "initializing";
        case SHEEN_DEVICE_READY: return "ready";
        case SHEEN_DEVICE_DEGRADED: return "degraded";
        case SHEEN_DEVICE_FAILED: return "failed";
        case SHEEN_DEVICE_REMOVED: return "removed";
    }
    return "failed";
}
