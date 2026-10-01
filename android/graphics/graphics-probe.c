#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-graphics.h"
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"usage: %s DRM_DEVICE\n",argv[0]);return 2;}
    int fd=open(argv[1],O_RDWR|O_CLOEXEC);
    if(fd<0){printf("{\"available\":false,\"device\":\"%s\"}\n",argv[1]);return 0;}
    printf("{\"available\":true,\"device\":\"%s\"}\n",argv[1]);
    close(fd);
    return 0;
}
