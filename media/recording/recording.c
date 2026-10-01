#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "sheen/recording.h"
struct sheen_recorder { int fd; char temp[4096]; char destination[4096]; char source[4096]; char container[32]; char id[65]; uint64_t bytes; sheen_recording_state state; };
static uint64_t hash64(const void *data,size_t n,uint64_t h){const unsigned char *p=data;for(size_t i=0;i<n;i++){h^=p[i];h*=1099511628211ULL;}return h;}
static void make_id(sheen_recorder *r){uint64_t a=hash64(r->destination,strlen(r->destination),1469598103934665603ULL);a=hash64(r->source,strlen(r->source),a);a=hash64(r->container,strlen(r->container),a);a=hash64(&r->bytes,sizeof(r->bytes),a);snprintf(r->id,sizeof(r->id),"%016llx",(unsigned long long)a);}
static int mkdir_parent(const char *path){char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return -1;memcpy(b,path,n+1);char *s=strrchr(b,'/');if(!s)return 0;*s=0;if(!b[0])return 0;for(char *p=b+1;*p;p++)if(*p=='/'){*p=0;mkdir(b,0755);*p='/';}if(mkdir(b,0755)<0&&errno!=EEXIST)return -1;return 0;}
sheen_recorder *sheen_recording_open(const char *destination,const char *source,const char *container){if(!destination||!source||!container)return NULL;sheen_recorder *r=calloc(1,sizeof(*r));if(!r)return NULL;r->fd=-1;r->state=SHEEN_RECORDING_ERROR;snprintf(r->destination,sizeof(r->destination),"%s",destination);snprintf(r->source,sizeof(r->source),"%s",source);snprintf(r->container,sizeof(r->container),"%s",container);if(mkdir_parent(destination)<0){free(r);return NULL;}snprintf(r->temp,sizeof(r->temp),"%s.part",destination);r->fd=open(r->temp,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC,0644);if(r->fd<0){free(r);return NULL;}r->state=SHEEN_RECORDING_ACTIVE;make_id(r);return r;}
int sheen_recording_write(sheen_recorder *r,const void *data,size_t bytes){if(!r||r->state!=SHEEN_RECORDING_ACTIVE||(!data&&bytes))return EINVAL;const unsigned char *p=data;size_t n=0;while(n<bytes){ssize_t w=write(r->fd,p+n,bytes-n);if(w<0){if(errno==EINTR)continue;r->state=SHEEN_RECORDING_ERROR;return errno;}if(w==0){r->state=SHEEN_RECORDING_ERROR;return EIO;}n+=(size_t)w;}r->bytes+=bytes;return 0;}
int sheen_recording_finalize(sheen_recorder *r){if(!r||r->state!=SHEEN_RECORDING_ACTIVE)return EINVAL;r->state=SHEEN_RECORDING_FINALIZING;if(fsync(r->fd)<0){r->state=SHEEN_RECORDING_ERROR;return errno;}if(close(r->fd)<0){r->fd=-1;r->state=SHEEN_RECORDING_ERROR;return errno;}r->fd=-1;if(rename(r->temp,r->destination)<0){r->state=SHEEN_RECORDING_ERROR;return errno;}r->state=SHEEN_RECORDING_COMPLETE;return 0;}
int sheen_recording_abort(sheen_recorder *r){if(!r)return EINVAL;if(r->fd>=0){close(r->fd);r->fd=-1;}unlink(r->temp);r->state=SHEEN_RECORDING_ABORTED;return 0;}
int sheen_recording_info_get(const sheen_recorder *r,sheen_recording_info *i){if(!r||!i)return EINVAL;memset(i,0,sizeof(*i));snprintf(i->id,sizeof(i->id),"%s",r->id);snprintf(i->source,sizeof(i->source),"%s",r->source);snprintf(i->destination,sizeof(i->destination),"%s",r->destination);snprintf(i->container,sizeof(i->container),"%s",r->container);i->bytes_written=r->bytes;i->state=r->state;return 0;}
void sheen_recording_close(sheen_recorder *r){if(!r)return;if(r->fd>=0)close(r->fd);if(r->state!=SHEEN_RECORDING_COMPLETE)unlink(r->temp);free(r);}
