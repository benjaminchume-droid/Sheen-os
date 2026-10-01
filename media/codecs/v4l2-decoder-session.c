#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
static unsigned int codec_fourcc(const char *id){if(!strcmp(id,"h264")||!strcmp(id,"avc"))return V4L2_PIX_FMT_H264;if(!strcmp(id,"hevc")||!strcmp(id,"h265"))return V4L2_PIX_FMT_HEVC;if(!strcmp(id,"vp8"))return V4L2_PIX_FMT_VP8;if(!strcmp(id,"vp9"))return V4L2_PIX_FMT_VP9;if(!strcmp(id,"av1"))return V4L2_PIX_FMT_AV1;if(!strcmp(id,"mpeg2"))return V4L2_PIX_FMT_MPEG2;return 0;}
static const char *fourcc_name(unsigned int f){switch(f){case V4L2_PIX_FMT_H264:return "h264";case V4L2_PIX_FMT_HEVC:return "hevc";case V4L2_PIX_FMT_VP8:return "vp8";case V4L2_PIX_FMT_VP9:return "vp9";case V4L2_PIX_FMT_AV1:return "av1";case V4L2_PIX_FMT_MPEG2:return "mpeg2";default:return "unknown";}}
static int has_output_format(int fd,unsigned int want){struct v4l2_fmtdesc x={0};x.type=V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;for(x.index=0;ioctl(fd,VIDIOC_ENUM_FMT,&x)==0;x.index++)if(x.pixelformat==want)return 1;x.type=V4L2_BUF_TYPE_VIDEO_OUTPUT;for(x.index=0;ioctl(fd,VIDIOC_ENUM_FMT,&x)==0;x.index++)if(x.pixelformat==want)return 1;return 0;}
int main(int argc,char **argv){if(argc!=2){fprintf(stderr,"usage: %s CODEC\\n",argv[0]);return 2;}unsigned int want=codec_fourcc(argv[1]);
if(!want){fprintf(stderr,"unsupported codec id: %s\\n",argv[1]);return 2;}
DIR *d=opendir("/dev");if(!d){printf("{\"available\":false,\"reason\":\"no-dev\"}\n");return 0;}
printf("{\"available\":false,\"codec\":");esc(argv[1]);printf(",\"fourcc\":\"%s\",\"devices\":[",fourcc_name(want));struct dirent *e;int first=1;
while((e=readdir(d))){if(strncmp(e->d_name,"video",5)!=0)continue;int oknum=1;for(const char *p=e->d_name+5;*p;p++)if(*p<'0'||*p>'9'){oknum=0;break;}if(!oknum)continue;char path[128];snprintf(path,sizeof(path),"/dev/%s",e->d_name);int fd=open(path,O_RDWR|O_CLOEXEC);if(fd<0)continue;struct v4l2_capability cap={0};if(ioctl(fd,VIDIOC_QUERYCAP,&cap)!=0){close(fd);continue;}unsigned int caps=(cap.device_caps&V4L2_CAP_DEVICE_CAPS)?cap.device_caps:cap.capabilities;int m2m=(caps&V4L2_CAP_VIDEO_M2M_MPLANE)||(caps&V4L2_CAP_VIDEO_M2M);if(!m2m||!has_output_format(fd,want)){close(fd);continue;}if(!first)putchar(',');first=0;printf("{\"path\":");esc(path);printf(",\"driver\":");esc((const char*)cap.driver);printf(",\"card\":");esc((const char*)cap.card);printf(",\"codec\":");esc(fourcc_name(want));printf("}");printf("");close(fd); }
closedir(d);printf("],\"selected\":%s}\n",first?"false":"true");return 0;}
