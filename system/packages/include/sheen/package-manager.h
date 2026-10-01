#ifndef SHEEN_PACKAGE_MANAGER_H
#define SHEEN_PACKAGE_MANAGER_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    char id[128];
    char version[64];
    char architecture[32];
    char installed_path[4096];
    uint64_t installed_size;
} sheen_package_info;
typedef struct sheen_package_manager sheen_package_manager;
sheen_package_manager *sheen_package_manager_open(const char *database,const char *root);
int sheen_package_manager_init(sheen_package_manager *manager);
int sheen_package_install(sheen_package_manager *manager,const char *archive,sheen_package_info *info);
int sheen_package_remove(sheen_package_manager *manager,const char *package_id);
int sheen_package_get(sheen_package_manager *manager,const char *package_id,sheen_package_info *info);
int sheen_package_count(sheen_package_manager *manager,uint64_t *count);
void sheen_package_manager_close(sheen_package_manager *manager);
#endif
