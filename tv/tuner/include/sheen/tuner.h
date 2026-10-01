#ifndef SHEEN_TUNER_H
#define SHEEN_TUNER_H
#include <stdint.h>
typedef enum { SHEEN_TUNER_DVBT, SHEEN_TUNER_DVBT2, SHEEN_TUNER_DVBC, SHEEN_TUNER_DVBS, SHEEN_TUNER_DVBS2, SHEEN_TUNER_ATSC, SHEEN_TUNER_ISDBT } sheen_tuner_delivery;
typedef struct { char name[128]; char type[64]; uint32_t min_frequency; uint32_t max_frequency; uint32_t frequency_step; uint32_t capabilities; } sheen_tuner_info;
typedef struct { uint32_t frequency_hz; uint32_t bandwidth_hz; sheen_tuner_delivery delivery; } sheen_tuner_tune;
typedef struct sheen_tuner sheen_tuner;
sheen_tuner *sheen_tuner_open(const char *device);
int sheen_tuner_info_get(sheen_tuner *t,sheen_tuner_info *info);
int sheen_tuner_tune(sheen_tuner *t,const sheen_tuner_tune *request);
int sheen_tuner_lock_status(sheen_tuner *t,uint32_t *status);
void sheen_tuner_close(sheen_tuner *t);
const char *sheen_tuner_delivery_name(sheen_tuner_delivery delivery);
#endif
