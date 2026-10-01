#include <stdio.h>
#include <string.h>
#include "sheen/android-permissions.h"
int main(int argc,char **argv){
    if(argc<4){fprintf(stderr,"usage: %s DB check|grant|revoke PACKAGE PERMISSION\n",argv[0]);return 2;}
    sheen_android_permissions *p=sheen_android_permissions_open(argv[1]);
    if(!p||sheen_android_permissions_init(p)){fprintf(stderr,"permission DB init failed\n");sheen_android_permissions_close(p);return 1;}
    const char *op=argv[2],*pkg=argv[3],*perm=argv[4];
    int rc=0,granted=0;
    if(!strcmp(op,"grant"))rc=sheen_android_permission_grant(p,pkg,perm);
    else if(!strcmp(op,"revoke"))rc=sheen_android_permission_revoke(p,pkg,perm);
    else if(!strcmp(op,"check"))rc=sheen_android_permission_check(p,pkg,perm,&granted);
    else rc=2;
    printf("{\"package_id\":\"%s\",\"permission\":\"%s\",\"capability\":%llu,\"granted\":%s,\"rc\":%d}\n",
           pkg,perm,(unsigned long long)sheen_android_permission_capability(perm),
           granted?"true":"false",rc);
    sheen_android_permissions_close(p);
    return rc?1:0;
}
