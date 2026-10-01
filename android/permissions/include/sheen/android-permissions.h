#ifndef SHEEN_ANDROID_PERMISSIONS_H
#define SHEEN_ANDROID_PERMISSIONS_H
#include <stdint.h>
typedef enum {
    SHEEN_ANDROID_CAP_NETWORK = 1ULL << 0,
    SHEEN_ANDROID_CAP_CAMERA = 1ULL << 1,
    SHEEN_ANDROID_CAP_MICROPHONE = 1ULL << 2,
    SHEEN_ANDROID_CAP_LOCATION = 1ULL << 3,
    SHEEN_ANDROID_CAP_STORAGE_READ = 1ULL << 4,
    SHEEN_ANDROID_CAP_STORAGE_WRITE = 1ULL << 5,
    SHEEN_ANDROID_CAP_BLUETOOTH = 1ULL << 6,
    SHEEN_ANDROID_CAP_NOTIFICATIONS = 1ULL << 7,
    SHEEN_ANDROID_CAP_WAKELOCK = 1ULL << 8,
    SHEEN_ANDROID_CAP_VIBRATE = 1ULL << 9
} sheen_android_capability;
typedef struct sheen_android_permissions sheen_android_permissions;
sheen_android_permissions *sheen_android_permissions_open(const char *path);
int sheen_android_permissions_init(sheen_android_permissions *policy);
uint64_t sheen_android_permission_capability(const char *permission);
int sheen_android_permission_grant(sheen_android_permissions *policy,const char *package_id,const char *permission);
int sheen_android_permission_revoke(sheen_android_permissions *policy,const char *package_id,const char *permission);
int sheen_android_permission_check(sheen_android_permissions *policy,const char *package_id,const char *permission,int *granted);
void sheen_android_permissions_close(sheen_android_permissions *policy);
#endif
