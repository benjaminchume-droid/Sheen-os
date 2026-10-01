#ifndef SHEEN_DVR_H
#define SHEEN_DVR_H
#include <stddef.h>
#include <stdint.h>
typedef enum { SHEEN_DVR_SCHEDULED, SHEEN_DVR_RECORDING, SHEEN_DVR_COMPLETED, SHEEN_DVR_FAILED, SHEEN_DVR_CANCELLED } sheen_dvr_state;
typedef struct { char recording_id[65]; char channel_id[65]; char title[256]; char output_path[4096]; int64_t start_ms; int64_t end_ms; sheen_dvr_state state; } sheen_dvr_job;
typedef struct sheen_dvr sheen_dvr;
sheen_dvr *sheen_dvr_open(const char *path);
int sheen_dvr_init(sheen_dvr *dvr);
int sheen_dvr_schedule(sheen_dvr *dvr,const sheen_dvr_job *job);
int sheen_dvr_get(sheen_dvr *dvr,const char *id,sheen_dvr_job *job);
int sheen_dvr_list_due(sheen_dvr *dvr,int64_t now_ms,sheen_dvr_job *jobs,size_t capacity,size_t *count);
int sheen_dvr_set_state(sheen_dvr *dvr,const char *id,sheen_dvr_state state);
int sheen_dvr_cancel(sheen_dvr *dvr,const char *id);
int sheen_dvr_count(sheen_dvr *dvr,uint64_t *count);
void sheen_dvr_close(sheen_dvr *dvr);
const char *sheen_dvr_state_name(sheen_dvr_state state);
#endif
