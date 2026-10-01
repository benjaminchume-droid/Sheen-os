#include <stdio.h>
#include <stdlib.h>
#include "sheen/stream.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s URI\\n",argv[0]);return 2;}sheen_stream *s=sheen_stream_open(argv[1]);if(!s){fprintf(stderr,"stream open failed\\n");return 1;}unsigned char b[4096];unsigned long long total=0;for(;;){ssize_t n=sheen_stream_read(s,b,sizeof(b));if(n<0){sheen_stream_close(s);fprintf(stderr,"stream read failed\\n");return 1;}if(n==0)break;total+=(unsigned long long)n;}printf("{\"uri\":\"%s\",\"bytes\":%llu,\"eof\":%s}\n",sheen_stream_uri(s),total,sheen_stream_eof(s)?"true":"false");sheen_stream_close(s);return 0;}
