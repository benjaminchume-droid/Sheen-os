#include <stdio.h>
#include <string.h>
#include "sheen/log.h"
static void usage(const char *p){fprintf(stderr,"usage: %s LEVEL COMPONENT MESSAGE\\n",p);}
int main(int argc,char **argv){if(argc!=4){usage(argv[0]);return 2;}sheen_log_level l;if(!strcmp(argv[1],"debug"))l=SHEEN_LOG_DEBUG;else if(!strcmp(argv[1],"info"))l=SHEEN_LOG_INFO;else if(!strcmp(argv[1],"warn"))l=SHEEN_LOG_WARN;else if(!strcmp(argv[1],"error"))l=SHEEN_LOG_ERROR;else if(!strcmp(argv[1],"fatal"))l=SHEEN_LOG_FATAL;else{usage(argv[0]);return 2;}int rc=sheen_log_write_default(l,argv[2],argv[3]);if(rc){fprintf(stderr,"log write failed: %d\\n",rc);return 1;}return 0;}
