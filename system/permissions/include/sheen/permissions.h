#ifndef SHEEN_PERMISSIONS_H
#define SHEEN_PERMISSIONS_H
#include <stdint.h>
typedef struct sheen_permissions sheen_permissions;
typedef enum {
    SHEEN_PERM_DISPLAY,
    SHEEN_PERM_AUDIO,
    SHEEN_PERM_INPUT,
    SHEEN_PERM_NETWORK,
    SHEEN_PERM_STORAGE,
    SHEEN_PERM_CAMERA,
    SHEEN_PERM_MICROPHONE,
    SHEEN_PERM_TUNER,
    SHEEN_PERM_CASTING,
    SHEEN_PERM_POWER
} sheen_permission;
sheen_permissions *sheen_permissions_open(const char *path);
int sheen_permissions_init(sheen_permissions *permissions);
int sheen_permissions_grant(sheen_permissions *permissions,const char *app_id,sheen_permission permission);
int sheen_permissions_revoke(sheen_permissions *permissions,const char *app_id,sheen_permission permission);
int sheen_permissions_check(sheen_permissions *permissions,const char *app_id,sheen_permission permission,int *granted);
const char *sheen_permission_name(sheen_permission permission);
void sheen_permissions_close(sheen_permissions *permissions);
#endif
