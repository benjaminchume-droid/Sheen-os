#include <stdio.h>
#include <stdlib.h>
#include "sheen/media-library.h"
int main(int argc,char **argv){if(argc<3){fprintf(stderr,"usage: %s DB PATH\\n",argv[0]);return 2;}sheen_media_library *l=sheen_media_library_open(argv[1]);if(!l||sheen_media_library_init(l)){fprintf(stderr,"library open/init failed\\n");sheen_media_library_close(l);return 1;}int rc=sheen_media_library_scan_tree(l,argv[2]);uint64_t count=0;sheen_media_library_count(l,&count);printf("{\"db\":\"%s\",\"root\":\"%s\",\"indexed\":%llu,\"scan_rc\":%d}\n",argv[1],argv[2],(unsigned long long)count,rc);sheen_media_library_close(l);return rc?1:0;}
