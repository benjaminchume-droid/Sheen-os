#ifndef SHEEN_CAST_SESSION_H
#define SHEEN_CAST_SESSION_H
#include <stddef.h>
#include <stdint.h>
typedef enum {
    SHEEN_CAST_SESSION_CREATED,
    SHEEN_CAST_SESSION_CONNECTING,
    SHEEN_CAST_SESSION_ACTIVE,
    SHEEN_CAST_SESSION_STOPPING,
    SHEEN_CAST_SESSION_STOPPED,
    SHEEN_CAST_SESSION_FAILED,
    SHEEN_CAST_SESSION_UNSUPPORTED
} sheen_cast_session_state;
typedef struct {
    char session_id[65];
    char device_id[256];
    char mode[32];
    sheen_cast_session_state state;
} sheen_cast_session_info;
typedef struct sheen_cast_session_manager sheen_cast_session_manager;
typedef struct {
    int (*open)(void **ctx,const sheen_cast_session_info *session,const char *options);
    int (*stop)(void *ctx);
    void (*close)(void *ctx);
} sheen_cast_session_backend;
sheen_cast_session_manager *sheen_cast_session_manager_create(void);
void sheen_cast_session_manager_destroy(sheen_cast_session_manager *manager);
int sheen_cast_session_create(sheen_cast_session_manager *manager,
                              const char *device_id,const char *mode,
                              const char *options,
                              const sheen_cast_session_backend *backend,
                              char out_session_id[65]);
int sheen_cast_session_stop(sheen_cast_session_manager *manager,const char *session_id);
int sheen_cast_session_get(sheen_cast_session_manager *manager,const char *session_id,
                           sheen_cast_session_info *info);
int sheen_cast_session_count(sheen_cast_session_manager *manager,size_t *count);
#endif
