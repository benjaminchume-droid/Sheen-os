#include <stdio.h>
#include <string.h>
#include "sheen/tuner.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s DVB_FRONTEND_DEVICE\\n",argv[0]);return 2;}sheen_tuner *t=sheen_tuner_open(argv[1]);if(!t){printf("{\"available\":false,\"device\":\"%s\"}\n",argv[1]);return 0;}sheen_tuner_info i;if(sheen_tuner_info_get(t,&i)){sheen_tuner_close(t);return 1;}uint32_t st=0;sheen_tuner_lock_status(t,&st);printf("{\"available\":true,\"device\":\"%s\",\"name\":\"%s\",\"min_frequency_hz\":%u,\"max_frequency_hz\":%u,\"frequency_step_hz\":%u,\"capabilities\":%u,\"lock_status\":%u}\n",argv[1],i.name,i.min_frequency,i.max_frequency,i.frequency_step,i.capabilities,st);sheen_tuner_close(t);return 0;}
