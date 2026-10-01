#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sheen/hal.h"

static const char *class_names[] = {"block","drm","net","sound","tty","input","backlight","graphics",NULL};

static void json_escape(const char *s) {
    putchar(34);
    for (; s && *s; ++s) {
        if (*s == 34 || *s == 92) { putchar(92); putchar(*s); }
        else if ((unsigned char)*s < 32) putchar(32);
        else putchar(*s);
    }
    putchar(34);
}

static int read_prop(const char *base, const char *name, char *buf, size_t len) {
    char path[SHEEN_HAL_PATH_MAX];
    if (sheen_hal_join_path(path, sizeof(path), base, name) != 0) return -1;
    return sheen_hal_read_text(path, buf, len);
}

static void emit_property(const char *key, const char *value, int *comma) {
    if (!value || !value[0]) return;
    printf("%s", *comma ? "," : "");
    json_escape(key);
    putchar(':');
    json_escape(value);
    *comma = 1;
}

static void emit_path_device(const char *group, const char *name, const char *path) {
    sheen_hal_device d;
    if (sheen_hal_populate_device(&d, group, name, path) != 0) return;
    char value[256];
    printf("{\"device_id\":"); json_escape(d.device_id);
    printf(",\"class\":"); json_escape(d.class_name);
    printf(",\"name\":"); json_escape(d.name);
    printf(",\"state\":"); json_escape(sheen_hal_state_name(d.state));
    printf(",\"properties\":{");
    int comma = 0;
    if (read_prop(path, "vendor", value, sizeof(value)) == 0) emit_property("vendor", value, &comma);
    if (read_prop(path, "device", value, sizeof(value)) == 0) emit_property("device", value, &comma);
    if (read_prop(path, "idVendor", value, sizeof(value)) == 0) emit_property("idVendor", value, &comma);
    if (read_prop(path, "idProduct", value, sizeof(value)) == 0) emit_property("idProduct", value, &comma);
    if (read_prop(path, "product", value, sizeof(value)) == 0) emit_property("product", value, &comma);
    if (read_prop(path, "manufacturer", value, sizeof(value)) == 0) emit_property("manufacturer", value, &comma);
    if (read_prop(path, "class", value, sizeof(value)) == 0) emit_property("class_code", value, &comma);
    emit_property("sysfs_path", d.sysfs_path, &comma);
    printf("}}\n");
}

static void scan_root(const char *root, const char *group) {
    DIR *dir = opendir(root);
    if (!dir) return;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        char path[SHEEN_HAL_PATH_MAX];
        if (sheen_hal_join_path(path, sizeof(path), root, entry->d_name) != 0) continue;
        struct stat st;
        if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode)) continue;
        emit_path_device(group, entry->d_name, path);
    }
    closedir(dir);
}

int main(void) {
    if (!sheen_hal_sysfs_available() || access("/sys/class", R_OK | X_OK) != 0) {
        fprintf(stderr, "sheen-hw-discover: Linux sysfs is unavailable\n");
        return 2;
    }
    for (size_t i = 0; class_names[i]; ++i) {
        char root[SHEEN_HAL_PATH_MAX];
        if (sheen_hal_join_path(root, sizeof(root), "/sys/class", class_names[i]) == 0) scan_root(root, class_names[i]);
    }
    scan_root("/sys/bus/pci/devices", "pci");
    scan_root("/sys/bus/usb/devices", "usb");
    return 0;
}
