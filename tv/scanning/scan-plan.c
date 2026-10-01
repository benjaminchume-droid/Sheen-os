#include <stdio.h>
#include "sheen/scan.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s PLAN_FILE\\n",argv[0]);return 2;}sheen_scan_plan p;int rc=sheen_scan_plan_load(argv[1],&p);if(rc){fprintf(stderr,"scan plan invalid: %d\\n",rc);return 1;}printf("{\"step_count\":%zu,\"steps\":[",p.count);for(size_t i=0;i<p.count;i++){if(i)putchar(',');printf("{\"frequency_hz\":%u,\"bandwidth_hz\":%u,\"delivery\":\"%s\"}",p.steps[i].frequency_hz,p.steps[i].bandwidth_hz,sheen_tuner_delivery_name(p.steps[i].delivery));}printf("]}\n");return 0;}
