#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/prctl.h>
#include <prctl.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "sheen/app-sandbox.h"

struct sheen_sandbox {
    sheen_sandbox_config config;
    pid_t pid;
};

int sheen_sandbox_verify(const sheen_sandbox_config *c){
    if(!c||!c->rootfs[0]||!c->executable[0])return EINVAL;
    struct stat st;
    if(stat(c->rootfs,&st)!=0)return errno;
    if(!S_ISDIR(st.st_mode))return ENOTDIR;
    char path[4096];
    const char *rel=c->executable[0]=='/'?c->executable+1:c->executable;
    if(snprintf(path,sizeof(path),"%s/%s",c->rootfs,rel)<0)return EOVERFLOW;
    if(access(path,X_OK)!=0)return errno;
    return 0;
}

static int child_exec(sheen_sandbox *s,char *const argv[]){
    unsigned long flags=CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUTS;
    if(!s->config.network_enabled)flags|=CLONE_NEWNET;
    if(unshare(flags)<0)_exit(125);
    if(mount(NULL,"/",NULL,MS_REC|MS_PRIVATE,NULL)<0)_exit(126);

    pid_t inner=fork();
    if(inner<0)_exit(127);
    if(inner>0){
        int status=0;
        while(waitpid(inner,&status,0)<0&&errno==EINTR){}
        _exit(WIFEXITED(status)?WEXITSTATUS(status):128+WTERMSIG(status));
    }

    if(prctl(PR_SET_NO_NEW_PRIVS,1,0,0,0)<0)_exit(128);

    if(s->config.cpu_limit_seconds){
        struct rlimit r={s->config.cpu_limit_seconds,s->config.cpu_limit_seconds};
        if(setrlimit(RLIMIT_CPU,&r)<0)_exit(129);
    }
    if(s->config.memory_limit_bytes){
        struct rlimit r={s->config.memory_limit_bytes,s->config.memory_limit_bytes};
        if(setrlimit(RLIMIT_AS,&r)<0)_exit(130);
    }

    if(chdir(s->config.rootfs)<0)_exit(131);
    if(chroot(s->config.rootfs)<0)_exit(132);
    if(chdir("/")<0)_exit(133);

    mkdir("/proc",0555);
    mkdir("/tmp",01777);
    mount("proc","/proc","proc",MS_NOSUID|MS_NODEV|MS_NOEXEC,NULL);

    execv(s->config.executable,argv);
    _exit(134);
}

sheen_sandbox *sheen_sandbox_create(const sheen_sandbox_config *c){
    if(!c)return NULL;
    if(sheen_sandbox_verify(c))return NULL;
    sheen_sandbox *s=calloc(1,sizeof(*s));
    if(!s)return NULL;
    s->config=*c;s->pid=-1;return s;
}
int sheen_sandbox_start(sheen_sandbox *s,char *const argv[]){
    if(!s||!argv||!argv[0]||s->pid>0)return EINVAL;
    pid_t p=fork();if(p<0)return errno;
    if(p==0)child_exec(s,argv);
    s->pid=p;return 0;
}
int sheen_sandbox_stop(sheen_sandbox *s){
    if(!s)return EINVAL;
    if(s->pid<=0)return 0;
    if(kill(s->pid,SIGTERM)<0&&errno!=ESRCH)return errno;
    int status=0;
    if(waitpid(s->pid,&status,0)<0&&errno!=ECHILD)return errno;
    s->pid=-1;return 0;
}
int sheen_sandbox_status(const sheen_sandbox *s,pid_t *pid){
    if(!s||!pid)return EINVAL;*pid=s->pid;return 0;
}
void sheen_sandbox_destroy(sheen_sandbox *s){
    if(!s)return;
    if(s->pid>0)sheen_sandbox_stop(s);
    free(s);
}
