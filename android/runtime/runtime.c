#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "sheen/android-runtime.h"

struct sheen_android_runtime {
    sheen_android_runtime_config config;
    sheen_android_runtime_state state;
    pid_t supervisor_pid;
};

static uint64_t hash64(const void *data,size_t n,uint64_t h){
    const unsigned char *p=data;
    for(size_t i=0;i<n;i++){h^=p[i];h*=1099511628211ULL;}
    return h;
}
static void make_id(const char *root,char out[65]){
    uint64_t h=hash64(root,strlen(root),1469598103934665603ULL);
    snprintf(out,65,"%016llx",(unsigned long long)h);
}
static int path_exec(const char *root,const char *relative){
    char path[4096];
    if(snprintf(path,sizeof(path),"%s/%s",root,relative)<0) return -1;
    return access(path,X_OK)==0?0:-1;
}
static int path_dir(const char *root,const char *relative){
    char path[4096];
    if(snprintf(path,sizeof(path),"%s/%s",root,relative)<0) return -1;
    struct stat st;
    return stat(path,&st)==0 && S_ISDIR(st.st_mode) ? 0 : -1;
}
int sheen_android_runtime_verify(const sheen_android_runtime_config *c){
    if(!c||!c->rootfs[0]||!c->init_path[0])return EINVAL;
    struct stat st;
    if(stat(c->rootfs,&st)!=0)return errno;
    if(!S_ISDIR(st.st_mode))return ENOTDIR;
    if(path_exec(c->rootfs,c->init_path[0]=='/'?c->init_path+1:c->init_path)!=0)return ENOEXEC;
    if(path_dir(c->rootfs,"system")!=0)return ENOENT;
    if(path_dir(c->rootfs,"vendor")!=0)return ENOENT;
    if(path_dir(c->rootfs,"apex")!=0)return ENOENT;
    return 0;
}
static int mkdir_p(const char *path){
    char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return -1;
    memcpy(b,path,n+1);
    for(size_t i=1;i<n;i++)if(b[i]=='/'){
        b[i]=0;if(mkdir(b,0755)<0&&errno!=EEXIST)return -1;b[i]='/';
    }
    return mkdir(b,0755)<0&&errno!=EEXIST?-1:0;
}
static int child_runtime(sheen_android_runtime *r){
    if(unshare(CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUTS)<0)_exit(125);
    if(mount(NULL,"/",NULL,MS_REC|MS_PRIVATE,NULL)<0)_exit(126);

    pid_t child=fork();
    if(child<0)_exit(127);
    if(child>0){
        int status=0;
        while(waitpid(child,&status,0)<0&&errno==EINTR){}
        _exit(WIFEXITED(status)?WEXITSTATUS(status):128+WTERMSIG(status));
    }

    if(sethostname("sheen-android",13)<0){}
    if(chdir(r->config.rootfs)<0)_exit(128);
    if(chroot(r->config.rootfs)<0)_exit(129);
    if(chdir("/")<0)_exit(130);

    mkdir("/proc",0555);
    mkdir("/sys",0555);
    mkdir("/dev",0755);
    mkdir("/dev/pts",0755);
    mount("proc","/proc","proc",MS_NOSUID|MS_NODEV|MS_NOEXEC,NULL);
    mount("sysfs","/sys","sysfs",MS_NOSUID|MS_NODEV|MS_NOEXEC,NULL);

    char *const argv[]={(char *)r->config.init_path,NULL};
    char *const envp[]={
        "PATH=/system/bin:/system/xbin:/vendor/bin",
        "HOME=/",
        "SHELL=/system/bin/sh",
        "SHEEN_ANDROID_RUNTIME=1",
        NULL
    };
    execve(r->config.init_path,argv,envp);
    _exit(131);
}
sheen_android_runtime *sheen_android_runtime_create(const sheen_android_runtime_config *c){
    if(!c)return NULL;
    sheen_android_runtime *r=calloc(1,sizeof(*r));if(!r)return NULL;
    r->config=*c;
    if(!r->config.runtime_id[0])make_id(r->config.rootfs,r->config.runtime_id);
    r->state=SHEEN_ANDROID_RUNTIME_UNAVAILABLE;
    r->supervisor_pid=-1;
    if(sheen_android_runtime_verify(c)==0)r->state=SHEEN_ANDROID_RUNTIME_READY;
    return r;
}
int sheen_android_runtime_start(sheen_android_runtime *r){
    if(!r)return EINVAL;
    if(r->state!=SHEEN_ANDROID_RUNTIME_READY&&r->state!=SHEEN_ANDROID_RUNTIME_STOPPED)return EINVAL;
    if(sheen_android_runtime_verify(&r->config))return ENOEXEC;
    if(r->config.data_root[0]&&mkdir_p(r->config.data_root)<0)return errno;
    r->state=SHEEN_ANDROID_RUNTIME_STARTING;
    pid_t pid=fork();if(pid<0){r->state=SHEEN_ANDROID_RUNTIME_FAILED;return errno;}
    if(pid==0)child_runtime(r);
    r->supervisor_pid=pid;
    r->state=SHEEN_ANDROID_RUNTIME_RUNNING;
    return 0;
}
int sheen_android_runtime_stop(sheen_android_runtime *r){
    if(!r)return EINVAL;
    if(r->supervisor_pid<=0){r->state=SHEEN_ANDROID_RUNTIME_STOPPED;return 0;}
    r->state=SHEEN_ANDROID_RUNTIME_STOPPING;
    kill(r->supervisor_pid,SIGTERM);
    int status=0;
    if(waitpid(r->supervisor_pid,&status,0)<0&&errno!=ECHILD){r->state=SHEEN_ANDROID_RUNTIME_FAILED;return errno;}
    r->supervisor_pid=-1;
    r->state=SHEEN_ANDROID_RUNTIME_STOPPED;
    return 0;
}
int sheen_android_runtime_status(const sheen_android_runtime *r,sheen_android_runtime_state *state,pid_t *pid){
    if(!r||!state||!pid)return EINVAL;
    *state=r->state;*pid=r->supervisor_pid;return 0;
}
void sheen_android_runtime_destroy(sheen_android_runtime *r){
    if(!r)return;
    if(r->supervisor_pid>0)sheen_android_runtime_stop(r);
    free(r);
}
const char *sheen_android_runtime_state_name(sheen_android_runtime_state s){
    switch(s){
        case SHEEN_ANDROID_RUNTIME_UNAVAILABLE:return "unavailable";
        case SHEEN_ANDROID_RUNTIME_READY:return "ready";
        case SHEEN_ANDROID_RUNTIME_STARTING:return "starting";
        case SHEEN_ANDROID_RUNTIME_RUNNING:return "running";
        case SHEEN_ANDROID_RUNTIME_STOPPING:return "stopping";
        case SHEEN_ANDROID_RUNTIME_STOPPED:return "stopped";
        case SHEEN_ANDROID_RUNTIME_FAILED:return "failed";
    }
    return "failed";
}
