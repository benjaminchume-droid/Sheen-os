#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "sheen/app-lifecycle.h"

#define SHEEN_MAX_APPS 128

typedef struct {
    sheen_app_info info;
} app_slot;

struct sheen_app_manager {
    app_slot slots[SHEEN_MAX_APPS];
};

static app_slot *find_app(sheen_app_manager *m,const char *id){
    if(!m||!id)return NULL;
    for(size_t i=0;i<SHEEN_MAX_APPS;i++)
        if(m->slots[i].info.app_id[0]&&!strcmp(m->slots[i].info.app_id,id))
            return &m->slots[i];
    return NULL;
}

sheen_app_manager *sheen_app_manager_create(void){
    return calloc(1,sizeof(sheen_app_manager));
}

int sheen_app_launch(sheen_app_manager *m,const char *id,const char *exe,char *const argv[]){
    if(!m||!id||!exe||!id[0]||!exe[0]||!argv||!argv[0])return EINVAL;
    if(find_app(m,id))return EEXIST;
    app_slot *slot=NULL;
    for(size_t i=0;i<SHEEN_MAX_APPS;i++)if(!m->slots[i].info.app_id[0]){slot=&m->slots[i];break;}
    if(!slot)return ENOSPC;
    memset(slot,0,sizeof(*slot));
    snprintf(slot->info.app_id,sizeof(slot->info.app_id),"%s",id);
    snprintf(slot->info.executable,sizeof(slot->info.executable),"%s",exe);
    slot->info.state=SHEEN_APP_STARTING;

    pid_t pid=fork();
    if(pid<0){slot->info.state=SHEEN_APP_FAILED;return errno;}
    if(pid==0){
        if(setpgid(0,0)<0)_exit(126);
        int nullfd=open("/dev/null",O_RDWR|O_CLOEXEC);
        if(nullfd>=0){
            dup2(nullfd,STDIN_FILENO);
            dup2(nullfd,STDOUT_FILENO);
            dup2(nullfd,STDERR_FILENO);
            if(nullfd>2)close(nullfd);
        }
        execv(exe,argv);
        _exit(127);
    }
    slot->info.pid=pid;
    slot->info.state=SHEEN_APP_RUNNING;
    return 0;
}

int sheen_app_pause(sheen_app_manager *m,const char *id){
    app_slot *s=find_app(m,id);if(!s)return ENOENT;
    if(s->info.state!=SHEEN_APP_RUNNING)return EINVAL;
    if(kill(s->info.pid,SIGSTOP)<0)return errno;
    s->info.state=SHEEN_APP_PAUSED;
    return 0;
}
int sheen_app_resume(sheen_app_manager *m,const char *id){
    app_slot *s=find_app(m,id);if(!s)return ENOENT;
    if(s->info.state!=SHEEN_APP_PAUSED)return EINVAL;
    if(kill(s->info.pid,SIGCONT)<0)return errno;
    s->info.state=SHEEN_APP_RUNNING;
    return 0;
}
int sheen_app_stop(sheen_app_manager *m,const char *id){
    app_slot *s=find_app(m,id);if(!s)return ENOENT;
    if(s->info.pid<=0)return 0;
    if(s->info.state==SHEEN_APP_STOPPED||s->info.state==SHEEN_APP_CRASHED)return 0;
    s->info.state=SHEEN_APP_STOPPING;
    if(kill(-s->info.pid,SIGTERM)<0 && errno!=ESRCH)return errno;
    return 0;
}
int sheen_app_reap(sheen_app_manager *m){
    if(!m)return EINVAL;
    for(size_t i=0;i<SHEEN_MAX_APPS;i++){
        app_slot *s=&m->slots[i];
        if(!s->info.pid)continue;
        int status=0;
        pid_t r=waitpid(s->info.pid,&status,WNOHANG);
        if(r<=0)continue;
        s->info.pid=0;
        if(WIFEXITED(status)){
            s->info.exit_code=WEXITSTATUS(status);
            s->info.state=s->info.exit_code==0?SHEEN_APP_STOPPED:SHEEN_APP_CRASHED;
        }else if(WIFSIGNALED(status)){
            s->info.term_signal=WTERMSIG(status);
            s->info.state=SHEEN_APP_CRASHED;
        }
    }
    return 0;
}
int sheen_app_get(sheen_app_manager *m,const char *id,sheen_app_info *out){
    if(!m||!id||!out)return EINVAL;
    (void)sheen_app_reap(m);
    app_slot *s=find_app(m,id);if(!s)return ENOENT;
    *out=s->info;return 0;
}
int sheen_app_count(sheen_app_manager *m,size_t *count){
    if(!m||!count)return EINVAL;
    (void)sheen_app_reap(m);
    *count=0;
    for(size_t i=0;i<SHEEN_MAX_APPS;i++)
        if(m->slots[i].info.app_id[0]&&
           m->slots[i].info.state!=SHEEN_APP_STOPPED&&
           m->slots[i].info.state!=SHEEN_APP_CRASHED)
            (*count)++;
    return 0;
}
void sheen_app_manager_destroy(sheen_app_manager *m){
    if(!m)return;
    for(size_t i=0;i<SHEEN_MAX_APPS;i++)
        if(m->slots[i].info.pid>0)kill(-m->slots[i].info.pid,SIGTERM);
    for(size_t i=0;i<SHEEN_MAX_APPS;i++)
        if(m->slots[i].info.pid>0)waitpid(m->slots[i].info.pid,NULL,0);
    free(m);
}
const char *sheen_app_state_name(sheen_app_state s){
    switch(s){
        case SHEEN_APP_CREATED:return "created";
        case SHEEN_APP_STARTING:return "starting";
        case SHEEN_APP_RUNNING:return "running";
        case SHEEN_APP_PAUSED:return "paused";
        case SHEEN_APP_STOPPING:return "stopping";
        case SHEEN_APP_STOPPED:return "stopped";
        case SHEEN_APP_CRASHED:return "crashed";
        case SHEEN_APP_FAILED:return "failed";
    }
    return "failed";
}
