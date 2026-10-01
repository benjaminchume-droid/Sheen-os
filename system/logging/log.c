#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "sheen/log.h"

static int mkdir_parent(const char *path){char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return -1;memcpy(b,path,n+1);char *s=strrchr(b,'/');if(!s)return 0;*s=0;if(!b[0])return 0;for(char *p=b+1;*p;p++)if(*p=='/'){*p=0;mkdir(b,0755);*p='/';}if(mkdir(b,0755)<0&&errno!=EEXIST)return -1;return 0;}
static void escape(char *out,size_t n,const char *s){size_t p=0;for(size_t i=0;s&&s[i]&&p+2<n;i++){unsigned char c=s[i];if(c==34||c==92){if(p+2>=n)break;out[p++]=92;out[p++]=(char)c;}else if(c<32)out[p++]=32;else out[p++]=(char)c;}out[p]=0;}
const char *sheen_log_level_name(sheen_log_level l){switch(l){case SHEEN_LOG_DEBUG:return "debug";case SHEEN_LOG_INFO:return "info";case SHEEN_LOG_WARN:return "warn";case SHEEN_LOG_ERROR:return "error";case SHEEN_LOG_FATAL:return "fatal";}return "unknown";}
int sheen_log_write_path(const char *path,sheen_log_level level,const char *component,const char *message){
    if(!path||!component||!message)return EINVAL;if(mkdir_parent(path)<0)return errno;
    int fd=open(path,O_WRONLY|O_CREAT|O_APPEND|O_CLOEXEC,0640);if(fd<0)return errno;
    struct timespec ts;clock_gettime(CLOCK_REALTIME,&ts);struct tm tmv;gmtime_r(&ts.tv_sec,&tmv);
    char comp[512],msg[4096],line[5200];escape(comp,sizeof(comp),component);escape(msg,sizeof(msg),message);
    int n=snprintf(line,sizeof(line),"{\"timestamp\":\"%04d-%02d-%02dT%02d:%02d:%02dZ\",\"level\":\"%s\",\"component\":%s,\"message\":%s}\n",tmv.tm_year+1900,tmv.tm_mon+1,tmv.tm_mday,tmv.tm_hour,tmv.tm_min,tmv.tm_sec,sheen_log_level_name(level),comp,msg);
    if(n<0||(size_t)n>=sizeof(line)){close(fd);return EOVERFLOW;}
    ssize_t w=write(fd,line,(size_t)n);int rc=(w==n)?0:(w<0?errno:EIO);close(fd);return rc;
}
int sheen_log_write_default(sheen_log_level level,const char *component,const char *message){const char *path=getenv("SHEEN_LOG_PATH");if(!path||!*path)path="/run/sheen/logs/system.jsonl";return sheen_log_write_path(path,level,component,message);}
