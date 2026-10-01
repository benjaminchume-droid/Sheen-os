#ifndef SHEEN_ANDROID_RUNTIME_H
#define SHEEN_ANDROID_RUNTIME_H
#include <stddef.h>
#include <sys/types.h>
typedef enum {
    SHEEN_ANDROID_RUNTIME_UNAVAILABLE,
    SHEEN_ANDROID_RUNTIME_READY,
    SHEEN_ANDROID_RUNTIME_STARTING,
    SHEEN_ANDROID_RUNTIME_RUNNING,
    SHEEN_ANDROID_RUNTIME_STOPPING,
    SHEEN_ANDROID_RUNTIME_STOPPED,
    SHEEN_ANDROID_RUNTIME_FAILED
} sheen_android_runtime_state;

typedef struct {
    char rootfs[4096];
    char data_root[4096];
    char init_path[512];
    char runtime_id[65];
} sheen_android_runtime_config;

typedef struct sheen_android_runtime sheen_android_runtime;

int sheen_android_runtime_verify(const sheen_android_runtime_config *config);
sheen_android_runtime *sheen_android_runtime_create(const sheen_android_runtime_config *config);
int sheen_android_runtime_start(sheen_android_runtime *runtime);
int sheen_android_runtime_stop(sheen_android_runtime *runtime);
int sheen_android_runtime_status(const sheen_android_runtime *runtime,
                                 sheen_android_runtime_state *state,
                                 pid_t *pid);
void sheen_android_runtime_destroy(sheen_android_runtime *runtime);
const char *sheen_android_runtime_state_name(sheen_android_runtime_state state);
#endif
