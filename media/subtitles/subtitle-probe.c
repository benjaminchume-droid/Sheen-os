#include <stdio.h>
#include <string.h>
#include "sheen/subtitle.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s FILE\\n",argv[0]);return 2;}sheen_subtitle_document d;int rc=sheen_subtitle_parse_file(argv[1],&d);if(rc){fprintf(stderr,"parse failed: %d\\n",rc);return 1;}printf("{\"format\":\"%s\",\"cue_count\":%zu,\"cues\":[",sheen_subtitle_format_name(d.format),d.cue_count);for(size_t i=0;i<d.cue_count;i++){if(i)putchar(',');printf("{\"start_ms\":%llu,\"end_ms\":%llu,\"text\":\"", (unsigned long long)d.cues[i].start_ms,(unsigned long long)d.cues[i].end_ms);for(const char *p=d.cues[i].text;*p;p++){if(*p==34||*p==92)putchar(92);if(*p==10)fputs("\\n",stdout);else putchar(*p);}printf("\"}");}printf("]}\n");return 0;}
