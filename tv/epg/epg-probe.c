#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/epg.h"
int main(int argc,char **argv){
    if(argc<2){fprintf(stderr,"usage: %s FILE [--dvb SERVICE_ID]\n",argv[0]);return 2;}
    sheen_epg_document d; int rc;
    if(argc==4 && !strcmp(argv[2],"--dvb")){
        unsigned long sid=strtoul(argv[3],NULL,10);
        if(sid>65535){fprintf(stderr,"invalid service id\n");return 2;}
        rc=sheen_epg_parse_dvb_ts(argv[1],(uint16_t)sid,&d);
    }else rc=sheen_epg_parse_xmltv(argv[1],&d);
    if(rc){fprintf(stderr,"epg parse failed: %d\n",rc);return 1;}
    printf("{"event_count":%zu,"events":[",d.event_count);
    for(size_t i=0;i<d.event_count;i++){
        if(i)putchar(',');
        printf("{"channel_id":"%s","service_id":%u,"event_id":%u,"start_ms":%lld,"end_ms":%lld,"running_status":%u,"free_ca_mode":%u,"title":"",
            d.events[i].channel_id,d.events[i].service_id,d.events[i].event_id,(long long)d.events[i].start_ms,(long long)d.events[i].end_ms,d.events[i].running_status,d.events[i].free_ca_mode);
        for(const char *q=d.events[i].title;*q;q++){if(*q=='"'||*q=='\\')putchar('\\');if(*q=='\n')fputs("\\n",stdout);else putchar(*q);}
        fputs("","description":"",stdout);
        for(const char *q=d.events[i].description;*q;q++){if(*q=='"'||*q=='\\')putchar('\\');if(*q=='\n')fputs("\\n",stdout);else putchar(*q);}
        fputs(""}",stdout);
    }
    puts("]}"); return 0;
}
