#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sheen/scan.h"
static char *trim(char *s){while(*s&&isspace((unsigned char)*s))s++;char *e=s+strlen(s);while(e>s&&isspace((unsigned char)e[-1]))*--e=0;return s;}
int sheen_scan_parse_delivery(const char *n,sheen_tuner_delivery *d){if(!n||!d)return EINVAL;if(!strcmp(n,"dvbt"))*d=SHEEN_TUNER_DVBT;else if(!strcmp(n,"dvbt2"))*d=SHEEN_TUNER_DVBT2;else if(!strcmp(n,"dvbc"))*d=SHEEN_TUNER_DVBC;else if(!strcmp(n,"dvbs"))*d=SHEEN_TUNER_DVBS;else if(!strcmp(n,"dvbs2"))*d=SHEEN_TUNER_DVBS2;else if(!strcmp(n,"atsc"))*d=SHEEN_TUNER_ATSC;else if(!strcmp(n,"isdbt"))*d=SHEEN_TUNER_ISDBT;else return EINVAL;return 0;}
int sheen_scan_plan_add(sheen_scan_plan *p,const sheen_scan_step *s){if(!p||!s||!s->frequency_hz||p->count>=SHEEN_SCAN_MAX_STEPS)return EINVAL;p->steps[p->count++]=*s;return 0;}
int sheen_scan_plan_load(const char *path,sheen_scan_plan *p){if(!path||!p)return EINVAL;memset(p,0,sizeof(*p));FILE *f=fopen(path,"r");if(!f)return errno;char line[512];size_t line_no=0;while(fgets(line,sizeof(line),f)){line_no++;char *s=trim(line);if(!*s||*s=='#')continue;unsigned long long freq=0,bw=0;char del[32];if(sscanf(s,"%llu %llu %31s",&freq,&bw,del)!=3){fclose(f);return EINVAL;}sheen_tuner_delivery d;if(sheen_scan_parse_delivery(del,&d)){fclose(f);return EINVAL;}sheen_scan_step step={(uint32_t)freq,(uint32_t)bw,d};if(sheen_scan_plan_add(p,&step)){fclose(f);return ENOSPC;}}int rc=ferror(f)?errno:0;fclose(f);return rc;}
int sheen_scan_run(sheen_tuner *t,const sheen_scan_plan *p,sheen_scan_result *r,size_t cap,size_t *count){if(!t||!p||!r||!count||cap<p->count)return EINVAL;*count=0;for(size_t i=0;i<p->count;i++){r[i].step=p->steps[i];r[i].tune_rc=sheen_tuner_tune(t,&r[i].step);r[i].lock_status=0;if(!r[i].tune_rc)sheen_tuner_lock_status(t,&r[i].lock_status);(*count)++;}return 0;}
