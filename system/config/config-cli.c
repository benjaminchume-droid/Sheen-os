#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sheen/config.h"
static void usage(const char *p){fprintf(stderr,"usage: %s --file PATH <get SECTION KEY | set SECTION KEY VALUE | dump>\\n",p);}
int main(int argc,char **argv){if(argc<4||strcmp(argv[1],"--file")){usage(argv[0]);return 2;}const char *file=argv[2];const char *op=argv[3];sheen_config *c=sheen_config_create();if(!c)return 1;int rc=sheen_config_load(c,file);if(rc&&!(rc==2&&(!strcmp(op,"set")))){fprintf(stderr,"load failed: %d\\n",rc);sheen_config_destroy(c);return 1;}
if(!strcmp(op,"get")&&argc==6){const char *v=sheen_config_get(c,argv[4],argv[5]);if(!v){sheen_config_destroy(c);return 1;}puts(v);}
else if(!strcmp(op,"set")&&argc==7){if((rc=sheen_config_set(c,argv[4],argv[5],argv[6]))==0)rc=sheen_config_save_atomic(c,file);if(rc)fprintf(stderr,"set failed: %d\\n",rc);}
else if(!strcmp(op,"dump")&&argc==4)rc=sheen_config_dump(c,STDOUT_FILENO);
else{usage(argv[0]);rc=2;}sheen_config_destroy(c);return rc?1:0;}
