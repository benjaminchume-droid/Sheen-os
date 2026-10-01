#ifndef SHEEN_ANDROID_PACKAGE_H
#define SHEEN_ANDROID_PACKAGE_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    char package_id[256];
    uint64_t version_code;
    char source_path[4096];
    char installed_path[4096];
    uint64_t size_bytes;
    uint8_t has_manifest;
    uint8_t has_dex;
    uint8_t has_tv_feature;
} sheen_android_package_info;
typedef struct sheen_android_package_manager sheen_android_package_manager;
sheen_android_package_manager *sheen_android_package_open(const char *database_path,
                                                           const char *package_root);
int sheen_android_package_init(sheen_android_package_manager *manager);
int sheen_android_package_inspect(const char *apk_path,sheen_android_package_info *info);
int sheen_android_package_install(sheen_android_package_manager *manager,
                                  const char *apk_path,
                                  sheen_android_package_info *info);
int sheen_android_package_remove(sheen_android_package_manager *manager,
                                 const char *package_id);
int sheen_android_package_get(sheen_android_package_manager *manager,
                              const char *package_id,
                              sheen_android_package_info *info);
int sheen_android_package_count(sheen_android_package_manager *manager,
                                uint64_t *count);
void sheen_android_package_close(sheen_android_package_manager *manager);
#endif
