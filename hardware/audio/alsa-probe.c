#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "sheen/hal.h"
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
static int read_attr(const char *base,const char *name,char *out,size_t len){char p[SHEEN_HAL_PATH_MAX];if(sheen_hal_join_path(p,sizeof(p),base,name)!=0)return -1;return sheen_hal_read_text(p,out,len);}
int main(void){
DIR *d=opendir("/sys/class/sound");if(!d){printf("{\"available\":false,\"devices\":[]}\n");return 0;}
printf("{\"available\":true,\"devices\":[");struct dirent *e;int first=1;
while((e=readdir(d))){if(!strncmp(e->d_name,"card",4)==0)continue;if(!strncmp(e->d_name,"control",7)&&e->d_name[0]){}
    if(strcmp(e->d_name,".")==0||strcmp(e->d_name,"..")==0)continue;
    char path[SHEEN_HAL_PATH_MAX],id[256]={0},uevent[512]={0},driver[256]={0};if(sheen_hal_join_path(path,sizeof(path),"/sys/class/sound",e->d_name)!=0)continue;
    if(strncmp(e->d_name,"card",4)!=0)continue;
    (void)read_attr(path,"id",id,sizeof(id));(void)read_attr(path,"uevent",uevent,sizeof(uevent));char drv[SHEEN_HAL_PATH_MAX];if(sheen_hal_join_path(drv,sizeof(drv),path,"device/driver")!=0)sprintf(drv,"");else (void)sheen_hal_read_link(drv,driver,sizeof(driver));
    if(!first)putchar(',');first=0;printf("{\"name\":");esc(e->d_name);printf(",\"id\":");esc(id);printf(",\"driver\":");esc(driver);printf(",\"uevent\":");esc(uevent);printf("}");
}
closedir(d);printf("]}\n");return 0;
}
