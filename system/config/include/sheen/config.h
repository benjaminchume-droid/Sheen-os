#ifndef SHEEN_CONFIG_H
#define SHEEN_CONFIG_H
#include <stddef.h>
#define SHEEN_CONFIG_SECTION_MAX 64
#define SHEEN_CONFIG_KEY_MAX 128
#define SHEEN_CONFIG_VALUE_MAX 1024
typedef struct sheen_config sheen_config;
sheen_config *sheen_config_create(void);
void sheen_config_destroy(sheen_config *config);
int sheen_config_load(sheen_config *config,const char *path);
const char *sheen_config_get(const sheen_config *config,const char *section,const char *key);
int sheen_config_set(sheen_config *config,const char *section,const char *key,const char *value);
int sheen_config_save_atomic(const sheen_config *config,const char *path);
int sheen_config_dump(const sheen_config *config,int fd);
#endif
