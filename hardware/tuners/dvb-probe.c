#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "sheen/hal.h"
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
static int read_attr(const char *base,const char *name,char *out,size_t len){char p[SHEEN_HAL_PATH_MAX];if(sheen_hal_join_path(p,sizeof(p),base,name)!=0)return -1;return sheen_hal_read_text(p,out,len);}
int main(void){
DIR *d=opendir("/sys/class/dvb");if(!d){printf("{\"available\":false,\"inputs\":[]}\n");return 0;}
printf("{\"available\":true,\"inputs\":[");struct dirent *e;int first=1;
while((e=readdir(d))){if(!strncmp(e->d_name,".",1)||!strncmp(e->d_name,"..",2))continue;
    char path[SHEEN_HAL_PATH_MAX],uevent[1024]={0};if(sheen_hal_join_path(path,sizeof(path),"/sys/class/dvb",e->d_name)!=0)continue;(void)read_attr(path,"uevent",uevent,sizeof(uevent));
    char node[128]={0};if(!strncmp(e->d_name,"dvb",3)){int adapter=-1,front=-1;if(sscanf(e->d_name,"dvb%d.frontend%d",&adapter,&front)==2)snprintf(node,sizeof(node),"/dev/dvb/adapter%d/frontend%d",adapter,front);}
    if(!first)putchar(',');first=0;printf("{\"class_name\":");esc(e->d_name);printf(",\"sysfs_path\":");esc(path);printf(",\"device_node\":");esc(node);printf(",\"accessible\":%s,\"uevent\":",access(node,F_OK)==0?"true":"false");esc(uevent);printf("}");
}
closedir(d);printf("]}\n");return 0;
}
