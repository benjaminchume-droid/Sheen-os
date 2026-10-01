#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t stopping=0;
static void on_signal(int sig){(void)sig;stopping=1;}

static int mkdir_p(const char *path){
    char buf[4096]; size_t n=strlen(path); if(n>=sizeof(buf)) return -1; memcpy(buf,path,n+1);
    for(size_t i=1;i<n;i++){ if(buf[i]=='/'){buf[i]=0; if(mkdir(buf,0755)<0 && errno!=EEXIST) return -1; buf[i]='/';} }
    if(mkdir(buf,0755)<0 && errno!=EEXIST) return -1; return 0;
}

static int write_snapshot(const char *discovery,const char *output){
    char dir[4096]; size_t n=strlen(output); if(n>=sizeof(dir)) return -1; memcpy(dir,output,n+1);
    char *slash=strrchr(dir,'/'); if(!slash) return -1; *slash=0; if(mkdir_p(dir)<0) return -1;
    char tmp[4096]; int m=snprintf(tmp,sizeof(tmp),"%s.tmp.%ld",output,(long)getpid()); if(m<0 || (size_t)m>=sizeof(tmp)) return -1;
    int fd[2]; if(pipe(fd)<0) return -1;
    pid_t pid=fork(); if(pid<0){close(fd[0]);close(fd[1]);return -1;}
    if(pid==0){
        close(fd[0]); if(dup2(fd[1],STDOUT_FILENO)<0)_exit(125); close(fd[1]);
        execl(discovery,"sheen-hw-discover",(char*)NULL); _exit(126);
    }
    close(fd[1]); FILE *out=fopen(tmp,"w"); if(!out){close(fd[0]);kill(pid,SIGTERM);waitpid(pid,NULL,0);return -1;}
    char buf[8192]; ssize_t r;
    while((r=read(fd[0],buf,sizeof(buf)))>0){ if(fwrite(buf,1,(size_t)r,out)!=(size_t)r){fclose(out);close(fd[0]);waitpid(pid,NULL,0);unlink(tmp);return -1;} }
    close(fd[0]); int status=0; waitpid(pid,&status,0);
    if(fclose(out)!=0 || !WIFEXITED(status) || WEXITSTATUS(status)!=0){unlink(tmp);return -1;}
    int ofd=open(tmp,O_RDONLY|O_CLOEXEC); if(ofd>=0){fsync(ofd);close(ofd);}
    if(rename(tmp,output)<0){unlink(tmp);return -1;}
    return 0;
}

static int open_uevent_socket(void){
    int s=socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT); if(s<0) return -1;
    struct sockaddr_nl addr; memset(&addr,0,sizeof(addr)); addr.nl_family=AF_NETLINK; addr.nl_groups=1;
    if(bind(s,(struct sockaddr*)&addr,sizeof(addr))<0){close(s);return -1;}
    return s;
}

static int is_relevant_event(const char *msg, ssize_t len){
    const char *action=NULL; for(ssize_t i=0;i<len;){ size_t l=strnlen(msg+i,(size_t)(len-i)); if(l==0){i++;continue;} if(!strncmp(msg+i,"ACTION=",7)) action=msg+i+7; i+=(ssize_t)l+1; }
    if(!action) return 0;
    return !strcmp(action,"add") || !strcmp(action,"remove") || !strcmp(action,"change") || !strcmp(action,"move") || !strcmp(action,"bind") || !strcmp(action,"unbind");
}

int main(int argc,char **argv){
    const char *output="/run/sheen/hardware/devices.jsonl"; const char *discovery="/usr/sbin/sheen-hw-discover"; int once=0;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--once")) once=1;
        else if(!strcmp(argv[i],"--output") && i+1<argc) output=argv[++i];
        else if(!strcmp(argv[i],"--discovery") && i+1<argc) discovery=argv[++i];
        else {fprintf(stderr,"usage: %s [--once] [--output PATH] [--discovery PATH]\\n",argv[0]);return 2;}
    }
    struct sigaction sa; memset(&sa,0,sizeof(sa)); sa.sa_handler=on_signal; sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM,&sa,NULL); sigaction(SIGINT,&sa,NULL);
    if(write_snapshot(discovery,output)<0){fprintf(stderr,"sheen-device-manager: initial discovery failed: %s\\n",strerror(errno));return 1;}
    if(once) return 0;
    int sock=open_uevent_socket(); if(sock<0){fprintf(stderr,"sheen-device-manager: netlink uevent socket failed: %s\\n",strerror(errno));return 1;}
    char msg[16384];
    while(!stopping){
        struct pollfd pfd={.fd=sock,.events=POLLIN}; int pr=poll(&pfd,1,1000); if(pr<0){if(errno==EINTR)continue;break;} if(pr==0)continue;
        if(!(pfd.revents&POLLIN))continue;
        ssize_t n=recv(sock,msg,sizeof(msg)-1,0); if(n<=0)continue; msg[n]=0;
        if(is_relevant_event(msg,n)) { if(write_snapshot(discovery,output)<0) fprintf(stderr,"sheen-device-manager: rescan failed: %s\\n",strerror(errno)); }
    }
    close(sock); return 0;
}
