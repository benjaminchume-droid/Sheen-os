#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-runtime.h"

int main(int argc,char **argv){
    if(argc<2){
        fprintf(stderr,"usage: %s verify ROOTFS [INIT] | status ROOTFS [INIT]\n",argv[0]);
        return 2;
    }
    sheen_android_runtime_config c={0};
    snprintf(c.rootfs,sizeof(c.rootfs),"%s",argv[1]);
    snprintf(c.init_path,sizeof(c.init_path),"%s",argc>=3?argv[2]:"/init");
    int rc=sheen_android_runtime_verify(&c);
    printf("{\"rootfs\":\"%s\",\"available\":%s,\"verify_rc\":%d}\n",
           c.rootfs,rc==0?"true":"false",rc);
    return rc?1:0;
}
