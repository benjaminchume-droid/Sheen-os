#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-package.h"
static void print_info(const sheen_android_package_info *i){
    printf("{\"package_id\":\"%s\",\"version_code\":%llu,\"source_path\":\"%s\",\"installed_path\":\"%s\",\"size_bytes\":%llu,\"has_manifest\":%s,\"has_dex\":%s,\"has_tv_feature\":%s}\n",
        i->package_id,(unsigned long long)i->version_code,i->source_path,i->installed_path,
        (unsigned long long)i->size_bytes,i->has_manifest?"true":"false",
        i->has_dex?"true":"false",i->has_tv_feature?"true":"false");
}
int main(int argc,char **argv){
    if(argc<3){fprintf(stderr,"usage: %s inspect APK | install DB ROOT APK | get DB ROOT PACKAGE | count DB ROOT | remove DB ROOT PACKAGE\n",argv[0]);return 2;}
    const char *op=argv[1];
    if(!strcmp(op,"inspect")&&argc==3){
        sheen_android_package_info i;int rc=sheen_android_package_inspect(argv[2],&i);
        if(rc){fprintf(stderr,"inspect failed: %d\n",rc);return 1;}
        print_info(&i);return 0;
    }
    if((!strcmp(op,"install")&&argc==5)||(!strcmp(op,"get")&&argc==4)||(!strcmp(op,"count")&&argc==4)||(!strcmp(op,"remove")&&argc==5)){
        const char *db=argv[2],*root=argv[3];
        sheen_android_package_manager *m=sheen_android_package_open(db,root);
        if(!m||sheen_android_package_init(m)){fprintf(stderr,"package manager init failed\n");sheen_android_package_close(m);return 1;}
        int rc=0;
        if(!strcmp(op,"install")){
            sheen_android_package_info i;rc=sheen_android_package_install(m,argv[4],&i);if(!rc)print_info(&i);
        }else if(!strcmp(op,"get")){
            sheen_android_package_info i;rc=sheen_android_package_get(m,argv[4],&i);if(!rc)print_info(&i);
        }else if(!strcmp(op,"remove")){
            rc=sheen_android_package_remove(m,argv[4]);
            printf("{\"package_id\":\"%s\",\"removed\":%s}\n",argv[4],rc==0?"true":"false");
        }else{
            uint64_t n=0;rc=sheen_android_package_count(m,&n);
            if(!rc)printf("{\"count\":%llu}\n",(unsigned long long)n);
        }
        sheen_android_package_close(m);return rc?1:0;
    }
    fprintf(stderr,"invalid arguments\n");return 2;
}
