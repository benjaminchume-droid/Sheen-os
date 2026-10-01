#ifndef SHEEN_SCAN_H
#define SHEEN_SCAN_H
#include <stddef.h>
#include <stdint.h>
#include "sheen/tuner.h"
#define SHEEN_SCAN_MAX_STEPS 4096
typedef struct { uint32_t frequency_hz; uint32_t bandwidth_hz; sheen_tuner_delivery delivery; } sheen_scan_step;
typedef struct { size_t count; sheen_scan_step steps[SHEEN_SCAN_MAX_STEPS]; } sheen_scan_plan;
typedef struct { sheen_scan_step step; int tune_rc; uint32_t lock_status; } sheen_scan_result;
int sheen_scan_plan_load(const char *path,sheen_scan_plan *plan);
int sheen_scan_plan_add(sheen_scan_plan *plan,const sheen_scan_step *step);
int sheen_scan_run(sheen_tuner *tuner,const sheen_scan_plan *plan,sheen_scan_result *results,size_t capacity,size_t *count);
int sheen_scan_parse_delivery(const char *name,sheen_tuner_delivery *delivery);
#endif
