#include <stdio.h>
#include "sheen/android-capabilities.h"
int main(void){
    sheen_android_capabilities c;
    if(sheen_android_capabilities_probe(&c))return 1;
    printf("{\"supported\":[");
    int first=1;
    for(int i=0;i<SHEEN_ANDROID_FEATURE_COUNT;i++){
        if(sheen_android_capability_supported(&c,(sheen_android_feature)i)){
            if(!first)putchar(',');
            first=0;
            printf("\"%s\"",sheen_android_feature_name((sheen_android_feature)i));
        }
    }
    printf("],\"available_now\":[");
    first=1;
    for(int i=0;i<SHEEN_ANDROID_FEATURE_COUNT;i++){
        if(sheen_android_capability_available(&c,(sheen_android_feature)i)){
            if(!first)putchar(',');
            first=0;
            printf("\"%s\"",sheen_android_feature_name((sheen_android_feature)i));
        }
    }
    puts("]}");
    return 0;
}
