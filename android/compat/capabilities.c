#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sheen/android-capabilities.h"

static int exists(const char *path){return access(path,F_OK)==0;}

int sheen_android_capabilities_probe(sheen_android_capabilities *c){
    if(!c)return 22;
    memset(c,0,sizeof(*c));

    c->supported =
        (1ULL<<SHEEN_ANDROID_FEATURE_RUNTIME) |
        (1ULL<<SHEEN_ANDROID_FEATURE_BINDER) |
        (1ULL<<SHEEN_ANDROID_FEATURE_GRAPHICS_DMABUF) |
        (1ULL<<SHEEN_ANDROID_FEATURE_INPUT_EVDEV) |
        (1ULL<<SHEEN_ANDROID_FEATURE_AUDIO_PCM) |
        (1ULL<<SHEEN_ANDROID_FEATURE_STORAGE_SANDBOX) |
        (1ULL<<SHEEN_ANDROID_FEATURE_PERMISSIONS) |
        (1ULL<<SHEEN_ANDROID_FEATURE_TV_PROFILE) |
        (1ULL<<SHEEN_ANDROID_FEATURE_APK_PACKAGE);

    if(exists("/usr/libexec/sheen-android-runtime")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_RUNTIME;
    if(exists("/dev/binder")||exists("/dev/binderfs/binder")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_BINDER;
    if(exists("/dev/dri")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_GRAPHICS_DMABUF;
    if(exists("/dev/input")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_INPUT_EVDEV;
    if(exists("/sys/class/sound")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_AUDIO_PCM;
    if(exists("/var/lib/sheen/android")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_STORAGE_SANDBOX;
    if(exists("/usr/sbin/sheen-android-permissions")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_PERMISSIONS;
    if(exists("/usr/lib/libsheen-android-tv.a")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_TV_PROFILE;
    if(exists("/usr/sbin/sheen-apk")) c->available_now|=1ULL<<SHEEN_ANDROID_FEATURE_APK_PACKAGE;
    return 0;
}
int sheen_android_capability_supported(const sheen_android_capabilities *c,sheen_android_feature f){
    return c&&f>=0&&f<SHEEN_ANDROID_FEATURE_COUNT&&(c->supported&(1ULL<<f))!=0;
}
int sheen_android_capability_available(const sheen_android_capabilities *c,sheen_android_feature f){
    return c&&f>=0&&f<SHEEN_ANDROID_FEATURE_COUNT&&(c->available_now&(1ULL<<f))!=0;
}
const char *sheen_android_feature_name(sheen_android_feature f){
    switch(f){
        case SHEEN_ANDROID_FEATURE_RUNTIME:return "runtime";
        case SHEEN_ANDROID_FEATURE_BINDER:return "binder";
        case SHEEN_ANDROID_FEATURE_GRAPHICS_DMABUF:return "graphics-dmabuf";
        case SHEEN_ANDROID_FEATURE_INPUT_EVDEV:return "input-evdev";
        case SHEEN_ANDROID_FEATURE_AUDIO_PCM:return "audio-pcm";
        case SHEEN_ANDROID_FEATURE_STORAGE_SANDBOX:return "storage-sandbox";
        case SHEEN_ANDROID_FEATURE_PERMISSIONS:return "permissions";
        case SHEEN_ANDROID_FEATURE_TV_PROFILE:return "tv-profile";
        case SHEEN_ANDROID_FEATURE_APK_PACKAGE:return "apk-package";
        case SHEEN_ANDROID_FEATURE_COUNT:return "count";
    }
    return "unknown";
}
