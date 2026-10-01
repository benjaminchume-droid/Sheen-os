#include <stdio.h>
#include <string.h>
#include "sheen/container.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s SOURCE\\n",argv[0]);return 2;}sheen_container_info i;int rc=sheen_container_inspect_file(argv[1],&i);if(rc){fprintf(stderr,"inspect failed: %d\\n",rc);return 1;}printf("{\"source\":\"%s\",\"container\":\"%s\",\"mime\":\"%s\",\"size_bytes\":%llu,\"bytes_inspected\":%zu,\"seekable\":%s}\n",i.source,i.name,i.mime,(unsigned long long)i.size_bytes,i.bytes_inspected,i.seekable?"true":"false");return 0;}
