#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include "sheen/android-input.h"
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"usage: %s EVENT_DEVICE\n",argv[0]);return 2;}
    int fd=open(argv[1],O_RDONLY|O_CLOEXEC);
    if(fd<0){printf("{\"available\":false,\"device\":\"%s\"}\n",argv[1]);return 0;}
    int flags=fcntl(fd,F_GETFL);
    if(flags>=0)fcntl(fd,F_SETFL,flags|O_NONBLOCK);
    printf("{\"available\":true,\"device\":\"%s\",\"events\":[",argv[1]);
    int first=1;
    for(int i=0;i<32;i++){
        sheen_android_input_event e;
        int rc=sheen_android_input_poll(fd,&e);
        if(rc==EAGAIN||rc==EWOULDBLOCK)break;
        if(rc)continue;
        if(!first)putchar(',');
        first=0;
        printf("{\"type\":%d,\"android_code\":%d,\"action\":%d,\"x\":%d,\"y\":%d,\"value\":%d,\"time_ns\":%lld}",
               e.type,e.android_code,e.action,e.x,e.y,e.value,(long long)e.time_ns);
    }
    puts("]}");
    close(fd);
    return 0;
}
