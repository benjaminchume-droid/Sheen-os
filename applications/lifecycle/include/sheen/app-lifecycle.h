#ifndef SHEEN_APP_LIFECYCLE_H
#define SHEEN_APP_LIFECYCLE_H
#include <stddef.h>
#include <sys/types.h>
typedef enum {
    SHEEN_APP_CREATED,
    SHEEN_APP_STARTING,
    SHEEN_APP_RUNNING,
    SHEEN_APP_PAUSED,
    SHEEN_APP_STOPPING,
    SHEEN_APP_STOPPED,
    SHEEN_APP_CRASHED,
    SHEEN_APP_FAILED
} sheen_app_state;
typedef struct {
    char app_id[128];
    char executable[4096];
    pid_t pid;
    sheen_app_state state;
    int exit_code;
    int term_signal;
} sheen_app_info;
typedef struct sheen_app_manager sheen_app_manager;
sheen_app_manager *sheen_app_manager_create(void);
int sheen_app_launch(sheen_app_manager *manager,const char *app_id,
                     const char *executable,char *const argv[]);
int sheen_app_pause(sheen_app_manager *manager,const char *app_id);
int sheen_app_resume(sheen_app_manager *manager,const char *app_id);
int sheen_app_stop(sheen_app_manager *manager,const char *app_id);
int sheen_app_reap(sheen_app_manager *manager);
int sheen_app_get(sheen_app_manager *manager,const char *app_id,sheen_app_info *info);
int sheen_app_count(sheen_app_manager *manager,size_t *count);
void sheen_app_manager_destroy(sheen_app_manager *manager);
const char *sheen_app_state_name(sheen_app_state state);
#endif
