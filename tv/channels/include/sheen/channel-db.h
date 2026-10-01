#ifndef SHEEN_CHANNEL_DB_H
#define SHEEN_CHANNEL_DB_H
#include <stddef.h>
#include <stdint.h>
typedef struct sheen_channel_db sheen_channel_db;
typedef struct { char channel_id[65]; uint16_t service_id; uint16_t program_number; uint16_t pmt_pid; uint16_t pcr_pid; uint32_t frequency_hz; uint32_t bandwidth_hz; char delivery[32]; char name[256]; char provider[256]; int enabled; } sheen_channel;
sheen_channel_db *sheen_channel_db_open(const char *path);
int sheen_channel_db_init(sheen_channel_db *db);
int sheen_channel_db_upsert(sheen_channel_db *db,const sheen_channel *channel);
int sheen_channel_db_count(sheen_channel_db *db,uint64_t *count);
int sheen_channel_db_get(sheen_channel_db *db,const char *channel_id,sheen_channel *channel);
void sheen_channel_db_close(sheen_channel_db *db);
#endif
