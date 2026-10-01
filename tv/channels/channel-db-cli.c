#include <stdio.h>
#include <string.h>
#include "sheen/channel-db.h"
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s DB\\n",argv[0]);return 2;}sheen_channel_db *d=sheen_channel_db_open(argv[1]);if(!d||sheen_channel_db_init(d)){fprintf(stderr,"channel db init failed\\n");sheen_channel_db_close(d);return 1;}uint64_t n=0;sheen_channel_db_count(d,&n);printf("{\"channels\":%llu}\n",(unsigned long long)n);sheen_channel_db_close(d);return 0;}
