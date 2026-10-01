#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_SERVICES 128
#define MAX_ARGS 16

typedef struct {
    char id[128];
    char exec_path[4096];
    char *args[MAX_ARGS];
    int arg_count;
    int enabled;
    int restart_on_failure;
    pid_t pid;
    int failures;
    time_t last_start;
} service_t;

static volatile sig_atomic_t stopping=0;
static service_t services[MAX_SERVICES];
static size_t service_count=0;

static void on_signal(int sig){(void)sig;stopping=1;}

static char *trim(char *s){while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n')s++;char *e=s+strlen(s);while(e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\r'||e[-1]=='\n'))*--e=0;return s;}

static int parse_bool(const char *s){return !strcmp(s,"1")||!strcmp(s,"true")||!strcmp(s,"yes")||!strcmp(s,"on");}

static int load_service(const char *path){
    if(service_count>=MAX_SERVICES)return -1;
    FILE *f=fopen(path,"r"); if(!f)return -1;
    service_t *s=&services[service_count]; memset(s,0,sizeof(*s)); s->enabled=0; s->restart_on_failure=1;
    char line[4096];
    while(fgets(line,sizeof(line),f)){
        char *p=strchr(line,'#'); if(p)*p=0; p=trim(line); if(!*p)continue;
        char *eq=strchr(p,'='); if(!eq)continue; *eq=0; char *key=trim(p); char *value=trim(eq+1);
        if(!strcmp(key,"id")) snprintf(s->id,sizeof(s->id),"%s",value);
        else if(!strcmp(key,"exec")) snprintf(s->exec_path,sizeof(s->exec_path),"%s",value);
        else if(!strcmp(key,"enabled")) s->enabled=parse_bool(value);
        else if(!strcmp(key,"restart")) s->restart_on_failure=!strcmp(value,"on-failure")||!strcmp(value,"always");
        else if(!strncmp(key,"arg",3)){ if(s->arg_count<MAX_ARGS-1) s->args[s->arg_count++]=strdup(value); }
    }
    fclose(f);
    if(!s->id[0]||!s->exec_path[0]){ for(int i=0;i<s->arg_count;i++)free(s->args[i]); memset(s,0,sizeof(*s)); return -1; }
    s->args[s->arg_count]=NULL; service_count++; return 0;
}

static int load_directory(const char *dir){
    DIR *d=opendir(dir); if(!d)return -1; struct dirent *e;
    while((e=readdir(d))){
        if(!strstr(e->d_name,".conf"))continue;
        char path[4096]; int n=snprintf(path,sizeof(path),"%s/%s",dir,e->d_name); if(n<0||(size_t)n>=sizeof(path))continue;
        load_service(path);
    }
    closedir(d); return 0;
}

static void clear_argv(service_t *s,char **argv){
    argv[0]=s->exec_path;
    for(int i=0;i<s->arg_count;i++)argv[i+1]=s->args[i];
    argv[s->arg_count+1]=NULL;
}

static void start_service(service_t *s){
    if(!s->enabled||s->pid>0)return;
    pid_t pid=fork();
    if(pid<0){s->failures++;return;}
    if(pid==0){
        char *argv[MAX_ARGS+2]; clear_argv(s,argv);
        int nullfd=open("/dev/null",O_RDWR|O_CLOEXEC); if(nullfd>=0){dup2(nullfd,STDIN_FILENO);dup2(nullfd,STDOUT_FILENO);dup2(nullfd,STDERR_FILENO);if(nullfd>2)close(nullfd);}
        execv(s->exec_path,argv); _exit(127);
    }
    s->pid=pid; s->last_start=time(NULL);
}

static void reap(void){
    for(;;){ int status; pid_t pid=waitpid(-1,&status,WNOHANG); if(pid<=0)break;
        for(size_t i=0;i<service_count;i++)if(services[i].pid==pid){
            services[i].pid=0; if(!WIFEXITED(status)||WEXITSTATUS(status)!=0)services[i].failures++; break;
        }
    }
}

static void stop_all(void){
    for(size_t i=0;i<service_count;i++)if(services[i].pid>0)kill(services[i].pid,SIGTERM);
    for(size_t i=0;i<service_count;i++)if(services[i].pid>0)waitpid(services[i].pid,NULL,0);
}

static int write_status(const char *path){
    char dir[4096]; snprintf(dir,sizeof(dir),"%s",path); char *slash=strrchr(dir,'/'); if(!slash)return -1; *slash=0;
    mkdir(dir,0755); char tmp[4096]; snprintf(tmp,sizeof(tmp),"%s.tmp.%ld",path,(long)getpid()); FILE *f=fopen(tmp,"w"); if(!f)return -1;
    for(size_t i=0;i<service_count;i++)fprintf(f,"{\"id\":\"%s\",\"enabled\":%s,\"pid\":%ld,\"failures\":%d}\n",services[i].id,services[i].enabled?"true":"false",(long)services[i].pid,services[i].failures);
    fflush(f); fsync(fileno(f)); fclose(f); if(rename(tmp,path)<0){unlink(tmp);return -1;} return 0;
}

int main(int argc,char **argv){
    const char *config="/etc/sheen/services"; const char *status="/run/sheen/services/status.jsonl"; int once=0;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--once"))once=1;
        else if(!strcmp(argv[i],"--config-dir")&&i+1<argc)config=argv[++i];
        else if(!strcmp(argv[i],"--status")&&i+1<argc)status=argv[++i];
        else {fprintf(stderr,"usage: %s [--once] [--config-dir DIR] [--status PATH]\\n",argv[0]);return 2;}
    }
    struct sigaction sa; memset(&sa,0,sizeof(sa)); sa.sa_handler=on_signal; sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM,&sa,NULL); sigaction(SIGINT,&sa,NULL);
    if(load_directory(config)<0){fprintf(stderr,"sheen-service-manager: cannot load %s: %s\\n",config,strerror(errno));return 1;}
    for(size_t i=0;i<service_count;i++)start_service(&services[i]);
    if(write_status(status)<0)fprintf(stderr,"sheen-service-manager: status publication failed: %s\\n",strerror(errno));
    if(once){reap();stop_all();return 0;}
    while(!stopping){
        reap();
        for(size_t i=0;i<service_count;i++) if(services[i].restart_on_failure && services[i].pid==0){time_t now=time(NULL);if(now-services[i].last_start>=1)start_service(&services[i]);}
        write_status(status);
        struct pollfd pfd={.fd=-1,.events=0}; poll(&pfd,0,500);
    }
    stop_all(); write_status(status);
    for(size_t i=0;i<service_count;i++)for(int j=0;j<service_count;j++){}
    return 0;
}
