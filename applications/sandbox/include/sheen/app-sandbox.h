#ifndef SHEEN_APP_SANDBOX_H
#define SHEEN_APP_SANDBOX_H
#include <stdint.h>
#include <sys/types.h>
typedef struct {
    char rootfs[4096];
    char executable[4096];
    int network_enabled;
    uint64_t memory_limit_bytes;
    uint64_t cpu_limit_seconds;
} sheen_sandbox_config;
typedef struct sheen_sandbox sheen_sandbox;
int sheen_sandbox_verify(const sheen_sandbox_config *config);
sheen_sandbox *sheen_sandbox_create(const sheen_sandbox_config *config);
int sheen_sandbox_start(sheen_sandbox *sandbox,char *const argv[]);
int sheen_sandbox_stop(sheen_sandbox *sandbox);
int sheen_sandbox_status(const sheen_sandbox *sandbox,pid_t *pid);
void sheen_sandbox_destroy(sheen_sandbox *sandbox);
#endif
