#ifndef SHEEN_NETWORK_TV_H
#define SHEEN_NETWORK_TV_H
#include <stddef.h>
#include <stdint.h>
#include "sheen/live.h"
#define SHEEN_NETWORK_TV_MAX_CHANNELS 256
typedef struct { char channel_id[65]; char name[256]; char uri[4096]; int enabled; } sheen_network_channel;
typedef struct { size_t count; sheen_network_channel channels[SHEEN_NETWORK_TV_MAX_CHANNELS]; } sheen_network_catalog;
int sheen_network_catalog_load(const char *path,sheen_network_catalog *catalog);
int sheen_network_catalog_get(const sheen_network_catalog *catalog,const char *channel_id,sheen_network_channel *channel);
int sheen_network_catalog_validate(const sheen_network_channel *channel);
sheen_live_session *sheen_network_open(const sheen_network_channel *channel);
#endif
