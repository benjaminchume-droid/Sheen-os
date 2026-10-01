#include <stdio.h>
#include "sheen/epg.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s XMLTV_FILE\\n",argv[0]);return 2;}sheen_epg_document d;int rc=sheen_epg_parse_xmltv(argv[1],&d);if(rc){fprintf(stderr,"epg parse failed: %d\\n",rc);return 1;}printf("{\"event_count\":%zu,\"events\":[",d.event_count);for(size_t i=0;i<d.event_count;i++){if(i)putchar(',');printf("{\"channel_id\":\"%s\",\"start_ms\":%lld,\"end_ms\":%lld,\"title\":\"%s\"}",d.events[i].channel_id,(long long)d.events[i].start_ms,(long long)d.events[i].end_ms,d.events[i].title);}printf("]}\n");return 0;}
