#include <stdio.h>
#include "sheen/android-binder.h"
int main(void){
    sheen_android_binder_info i;
    int rc=sheen_android_binder_probe(&i);
    printf("{\"available\":%s,\"path\":\"%s\",\"protocol_version\":%u,\"probe_rc\":%d}\n",
           i.available?"true":"false",i.path,i.protocol_version,rc);
    return rc && rc!=2 ? 1 : 0;
}
