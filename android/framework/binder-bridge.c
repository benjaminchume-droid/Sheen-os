#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/android/binder.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sheen/android-binder.h"

static const char *candidates[] = {
    "/dev/binder",
    "/dev/hwbinder",
    "/dev/vndbinder",
    "/dev/binderfs/binder",
    NULL
};

int sheen_android_binder_query_version(int fd,uint32_t *version) {
    if(fd<0||!version)return EINVAL;
    struct binder_version v={0};
    if(ioctl(fd,BINDER_VERSION,&v)<0)return errno;
    if(v.protocol_version<7)return ENOTSUP;
    *version=(uint32_t)v.protocol_version;
    return 0;
}

int sheen_android_binder_probe(sheen_android_binder_info *info) {
    if(!info)return EINVAL;
    memset(info,0,sizeof(*info));
    for(size_t i=0;candidates[i];i++) {
        int fd=open(candidates[i],O_RDWR|O_CLOEXEC);
        if(fd<0)continue;
        uint32_t version=0;
        int rc=sheen_android_binder_query_version(fd,&version);
        close(fd);
        if(rc)continue;
        snprintf(info->path,sizeof(info->path),"%s",candidates[i]);
        info->available=1;
        info->protocol_version=version;
        return 0;
    }
    return ENOENT;
}

int sheen_android_binder_open(const sheen_android_binder_info *info) {
    if(!info||!info->available||!info->path[0])return ENOENT;
    return open(info->path,O_RDWR|O_CLOEXEC);
}

int sheen_android_binder_mountfs(const char *mountpoint) {
    if(!mountpoint||!mountpoint[0])return EINVAL;
    if(mkdir(mountpoint,0755)<0&&errno!=EEXIST)return errno;
    if(mount("binder",mountpoint,"binder",MS_NOSUID|MS_NODEV,NULL)<0)return errno;
    return 0;
}
