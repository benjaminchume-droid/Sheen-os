#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
int main(void){
DIR *d=opendir("/sys/class/video4linux");if(!d){printf("{\"available\":false,\"devices\":[]}\n");return 0;}
printf("{\"available\":true,\"devices\":[");struct dirent *e;int first=1;
while((e=readdir(d))){if(strncmp(e->d_name,"video",5)!=0)continue;const char *n=e->d_name+5;if(!*n)continue;int digits=1;for(const char *p=n;*p;p++)if(*p<'0'||*p>'9'){digits=0;break;}if(!digits)continue;
char path[128];snprintf(path,sizeof(path),"/dev/video%s",n);int fd=open(path,O_RDWR|O_CLOEXEC);struct v4l2_capability cap={0};int rc=fd>=0&&ioctl(fd,VIDIOC_QUERYCAP,&cap)==0;unsigned int caps=rc?cap.device_caps:0;if(rc&&!(caps&V4L2_CAP_DEVICE_CAPS))caps=cap.capabilities;
if(fd>=0)close(fd);if(!first)putchar(',');first=0;printf("{\"path\":");esc(path);printf(",\"driver\":");esc((const char*)cap.driver);printf(",\"card\":");esc((const char*)cap.card);printf(",\"bus_info\":");esc((const char*)cap.bus_info);printf(",\"capture\":%s,\"output\":%s,\"m2m\":%s}",(caps&V4L2_CAP_VIDEO_CAPTURE_MPLANE)||(caps&V4L2_CAP_VIDEO_CAPTURE)?"true":"false",(caps&V4L2_CAP_VIDEO_OUTPUT_MPLANE)||(caps&V4L2_CAP_VIDEO_OUTPUT)?"true":"false",(caps&V4L2_CAP_VIDEO_M2M_MPLANE)||(caps&V4L2_CAP_VIDEO_M2M)?"true":"false");
}
closedir(d);printf("]}\n");return 0;
}
