#define _GNU_SOURCE
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "sheen/hal.h"
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
static int read_attr(const char *base,const char *name,char *out,size_t len){char p[SHEEN_HAL_PATH_MAX];if(sheen_hal_join_path(p,sizeof(p),base,name)!=0)return -1;return sheen_hal_read_text(p,out,len);}
int main(void){
DIR *d=opendir("/sys/bus/pci/devices");if(!d){printf("{\"available\":false,\"gpus\":[]}\n");return 0;}
printf("{\"available\":true,\"gpus\":[");struct dirent *e;int first=1;
while((e=readdir(d))){if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;char path[SHEEN_HAL_PATH_MAX],cls[64]={0},vendor[128]={0},device[128]={0},subsystem[128]={0},driver[128]={0};snprintf(path,sizeof(path),"/sys/bus/pci/devices/%s",e->d_name);if(read_attr(path,"class",cls,sizeof(cls))!=0)continue;if(strncmp(cls,"0x03",4)!=0)continue;sheen_hal_read_text(path,vendor,sizeof(vendor));sheen_hal_read_text(path,device,sizeof(device));char link[SHEEN_HAL_PATH_MAX];char drv[SHEEN_HAL_PATH_MAX];snprintf(link,sizeof(link),"%s/subsystem_device",path);(void)link;snprintf(drv,sizeof(drv),"%s/driver",path);if(sheen_hal_read_link(drv,driver,sizeof(driver))<0)driver[0]=0;if(sheen_hal_read_link("%s/subsystem",subsystem,sizeof(subsystem))<0)subsystem[0]=0;
    if(!first)putchar(',');first=0;printf("{\"device_id\":");esc(e->d_name);printf(",\"class_code\":");esc(cls);printf(",\"vendor\":");esc(vendor);printf(",\"device\":");esc(device);printf(",\"driver\":");esc(driver);printf("}");
}
closedir(d);printf("]}\n");return 0;
}
