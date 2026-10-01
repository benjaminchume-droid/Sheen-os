#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
int main(void){
DIR *d=opendir("/sys/class/input");if(!d){printf("{\"available\":false,\"devices\":[]}\n");return 0;}
printf("{\"available\":true,\"devices\":[");struct dirent *e;int first=1;
while((e=readdir(d))){if(strncmp(e->d_name,"event",5)!=0)continue;const char *n=e->d_name+5;if(!*n)continue;for(const char *p=n;*p;p++)if(*p<'0'||*p>'9'){n=NULL;break;}if(!n)continue;
    char dev[128];snprintf(dev,sizeof(dev),"/dev/input/%s",e->d_name);int fd=open(dev,O_RDONLY|O_NONBLOCK|O_CLOEXEC);
    char name[256]="";struct input_id id={0};unsigned char evbits[(EV_MAX+7)/8]={0};
    int accessible=fd>=0;if(fd>=0){ioctl(fd,EVIOCGNAME(sizeof(name)),name);ioctl(fd,EVIOCGID,&id);ioctl(fd,EVIOCGBIT(0,sizeof(evbits)),evbits);}
    int has_key=!!(evbits[EV_KEY/8]&(1u<<(EV_KEY%8)));int has_rel=!!(evbits[EV_REL/8]&(1u<<(EV_REL%8)));int has_abs=!!(evbits[EV_ABS/8]&(1u<<(EV_ABS%8)));
    if(fd>=0)close(fd);if(!first)putchar(',');first=0;
    printf("{\"event\":");esc(dev);printf(",\"accessible\":%s,\"name\":",accessible?"true":"false");esc(name);
    printf(",\"bus\":%u,\"vendor\":%u,\"product\":%u,\"version\":%u",id.bustype,id.vendor,id.product,id.version);
    printf(",\"key\":%s,\"relative\":%s,\"absolute\":%s}",has_key?"true":"false",has_rel?"true":"false",has_abs?"true":"false");
}
closedir(d);printf("]}\n");return 0;
}
