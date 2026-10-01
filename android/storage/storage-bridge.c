#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sheen/android-storage.h"

static int valid_relative(const char *path){
    if(!path||!*path||path[0]=='/'||path[0]=='\\')return 0;
    const char *p=path;
    while(*p){
        const char *end=strpbrk(p,"/\\");
        size_t len=end?(size_t)(end-p):strlen(p);
        if(len==0){p=end+1;continue;}
        if(len==1&&p[0]=='.')return 0;
        if(len==2&&p[0]=='.'&&p[1]=='.')return 0;
        if(end)p=end+1;else break;
    }
    return 1;
}
static const char *root_for(const sheen_android_storage *s,sheen_android_storage_kind k){
    switch(k){
        case SHEEN_ANDROID_STORAGE_PRIVATE:return s->private_root;
        case SHEEN_ANDROID_STORAGE_CACHE:return s->cache_root;
        case SHEEN_ANDROID_STORAGE_SHARED:return s->shared_root;
    }
    return NULL;
}
int sheen_android_storage_init(sheen_android_storage *s,const char *p,const char *c,const char *shared){
    if(!s||!p||!c||!shared||!p[0]||!c[0]||!shared[0])return EINVAL;
    memset(s,0,sizeof(*s));
    if(strlen(p)>=sizeof(s->private_root)||strlen(c)>=sizeof(s->cache_root)||strlen(shared)>=sizeof(s->shared_root))return ENAMETOOLONG;
    snprintf(s->private_root,sizeof(s->private_root),"%s",p);
    snprintf(s->cache_root,sizeof(s->cache_root),"%s",c);
    snprintf(s->shared_root,sizeof(s->shared_root),"%s",shared);
    return 0;
}
int sheen_android_storage_resolve(const sheen_android_storage *s,sheen_android_storage_kind kind,const char *relative,char *out,size_t n){
    if(!s||!out||n<2||!valid_relative(relative))return EINVAL;
    const char *root=root_for(s,kind);if(!root)return EINVAL;
    int rc=snprintf(out,n,"%s/%s",root,relative);if(rc<0||(size_t)rc>=n)return ENAMETOOLONG;
    return 0;
}
static int mkdir_p(const char *path){
    char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return ENAMETOOLONG;memcpy(b,path,n+1);
    for(size_t i=1;i<n;i++)if(b[i]=='/'){b[i]=0;if(mkdir(b,0755)<0&&errno!=EEXIST)return errno;b[i]='/';}
    if(mkdir(b,0755)<0&&errno!=EEXIST)return errno;
    return 0;
}
int sheen_android_storage_mkdirs(const sheen_android_storage *s,sheen_android_storage_kind kind,const char *relative){
    char path[4096];int rc=sheen_android_storage_resolve(s,kind,relative,path,sizeof(path));if(rc)return rc;return mkdir_p(path);
}
