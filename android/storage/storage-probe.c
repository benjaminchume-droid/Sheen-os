#include <stdio.h>
#include <string.h>
#include "sheen/android-storage.h"
int main(void){
    sheen_android_storage s;
    if(sheen_android_storage_init(&s,"/var/lib/sheen/android/apps","/var/cache/sheen/android","/var/lib/sheen/android/shared"))return 1;
    char path[4096];
    if(sheen_android_storage_resolve(&s,SHEEN_ANDROID_STORAGE_PRIVATE,"files/config.json",path,sizeof(path)))return 2;
    printf("{\"resolved\":\"%s\",\"traversal_allowed\":",path);
    int rc=sheen_android_storage_resolve(&s,SHEEN_ANDROID_STORAGE_PRIVATE,"../host-secret",path,sizeof(path));
    puts(rc==0?"true}":"false}");
    return rc==0?1:0;
}
