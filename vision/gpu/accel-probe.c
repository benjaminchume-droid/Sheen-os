#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/drm.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "sheen/vision-accel.h"

static uint64_t get_cap(int fd,uint64_t cap) {
    struct drm_get_cap request={0};
    request.capability=cap;
    if(ioctl(fd,DRM_IOCTL_GET_CAP,&request)<0) return 0;
    return request.value;
}

int sheen_vision_accel_probe(sheen_vision_accel_inventory *inventory) {
    if(!inventory) return -1;
    memset(inventory,0,sizeof(*inventory));
    DIR *dir=opendir("/dev/dri");
    if(!dir) return 0;

    struct dirent *entry;
    while((entry=readdir(dir)) && inventory->count<SHEEN_VISION_MAX_ACCEL) {
        if(strncmp(entry->d_name,"renderD",7)!=0) continue;

        char path[128];
        snprintf(path,sizeof(path),"/dev/dri/%s",entry->d_name);

        int fd=open(path,O_RDWR|O_CLOEXEC);
        sheen_vision_accel_device *device=&inventory->devices[inventory->count++];
        memset(device,0,sizeof(*device));
        snprintf(device->device,sizeof(device->device),"%s",path);

        if(fd<0) continue;
        device->accessible=1;
        device->prime_cap=get_cap(fd,DRM_CAP_PRIME);
        device->addfb2_modifiers=get_cap(fd,DRM_CAP_ADDFB2_MODIFIERS);
        device->dumb_buffers=get_cap(fd,DRM_CAP_DUMB_BUFFER);
#ifdef DRM_CAP_SYNCOBJ
        device->syncobj=get_cap(fd,DRM_CAP_SYNCOBJ);
#endif
        close(fd);
    }
    closedir(dir);
    return 0;
}
