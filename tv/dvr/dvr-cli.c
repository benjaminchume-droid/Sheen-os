#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/dvr.h"
int main(int argc,char **argv){
 if(argc<3){fprintf(stderr,"usage: %s DB schedule CHANNEL START_MS END_MS OUTPUT | due NOW_MS | count\n",argv[0]);return 2;}
 sheen_dvr *d=sheen_dvr_open(argv[1]);if(!d||sheen_dvr_init(d)){fprintf(stderr,"dvr init failed\n");sheen_dvr_close(d);return 1;}
 if(!strcmp(argv[2],"schedule")&&argc==7){sheen_dvr_job j={0};snprintf(j.channel_id,sizeof(j.channel_id),"%s",argv[3]);j.start_ms=strtoll(argv[4],NULL,10);j.end_ms=strtoll(argv[5],NULL,10);snprintf(j.output_path,sizeof(j.output_path),"%s",argv[6]);snprintf(j.title,sizeof(j.title),"%s","Scheduled recording");if(sheen_dvr_schedule(d,&j)){sheen_dvr_close(d);return 1;}printf("{\"recording_id\":\"%s\",\"state\":\"%s\"}\n",j.recording_id,sheen_dvr_state_name(j.state));}
 else if(!strcmp(argv[2],"due")&&argc==4){sheen_dvr_job jobs[64];size_t n=0;if(sheen_dvr_list_due(d,strtoll(argv[3],NULL,10),jobs,64,&n)){sheen_dvr_close(d);return 1;}printf("{\"count\":%zu,\"jobs\":[",n);for(size_t i=0;i<n;i++){if(i)putchar(',');printf("{\"recording_id\":\"%s\",\"channel_id\":\"%s\",\"start_ms\":%lld,\"end_ms\":%lld}",jobs[i].recording_id,jobs[i].channel_id,(long long)jobs[i].start_ms,(long long)jobs[i].end_ms);}puts("]}");}
 else if(!strcmp(argv[2],"count")&&argc==3){uint64_t n=0;sheen_dvr_count(d,&n);printf("{\"count\":%llu}\n",(unsigned long long)n);}
 else {fprintf(stderr,"invalid arguments\n");sheen_dvr_close(d);return 2;}
 sheen_dvr_close(d);return 0;
}
