#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
static int connect_bus(const char *path){int s=socket(AF_UNIX,SOCK_STREAM,0);if(s<0)return -1;struct sockaddr_un a;memset(&a,0,sizeof(a));a.sun_family=AF_UNIX;if(strlen(path)>=sizeof(a.sun_path)){close(s);return -1;}strcpy(a.sun_path,path);if(connect(s,(struct sockaddr*)&a,sizeof(a))<0){close(s);return -1;}return s;}
static int send_line(int fd,const char *line){size_t n=strlen(line),p=0;while(p<n){ssize_t w=send(fd,line+p,n-p,MSG_NOSIGNAL);if(w<0){if(errno==EINTR)continue;return -1;}if(w==0)return -1;p+=(size_t)w;}return 0;}
static int json_string(const char *line,const char *key,char *out,size_t n){char pat[160];int m=snprintf(pat,sizeof(pat),""%s":"",key);if(m<0||(size_t)m>=sizeof(pat)||n<2)return -1;const char *p=strstr(line,pat);if(!p)return -1;p+=m;size_t w=0;while(*p&&w+1<n){if(*p==92&&p[1]){if(p[1]==34||p[1]==92||p[1]==47){out[w++]=p[1];p+=2;continue;}if(p[1]=='n'){out[w++]=10;p+=2;continue;}}if(*p==34){out[w]=0;return 0;}out[w++]=*p++;}return -1;}
static int read_line(int fd,char *buf,size_t n){size_t p=0;while(p+1<n){char c;ssize_t r=recv(fd,&c,1,0);if(r<=0)return -1;buf[p++]=c;if(c==10){buf[p]=0;return (int)p;}}return -2;}
static int run_probe(const char *path,char *out,size_t out_len){
    int pipefd[2];if(pipe(pipefd)<0)return -1;pid_t pid=fork();if(pid<0){close(pipefd[0]);close(pipefd[1]);return -1;}
    if(pid==0){dup2(pipefd[1],STDOUT_FILENO);close(pipefd[0]);close(pipefd[1]);execl(path,"sheen-drm-probe",(char*)NULL);_exit(127);}
    close(pipefd[1]);size_t p=0;while(p+1<out_len){ssize_t r=read(pipefd[0],out+p,out_len-p-1);if(r<0){if(errno==EINTR)continue;break;}if(r==0)break;p+=(size_t)r;}close(pipefd[0]);out[p]=0;int st=0;waitpid(pid,&st,0);if(!WIFEXITED(st)||WEXITSTATUS(st)!=0)return -1;return p>0?0:-1;
}
int main(int argc,char **argv){
    const char *bus="/run/sheen/ipc/bus.sock";const char *probe="/usr/sbin/sheen-drm-probe";
    for(int i=1;i<argc;i++){if(!strcmp(argv[i],"--bus")&&i+1<argc)bus=argv[++i];else if(!strcmp(argv[i],"--probe")&&i+1<argc)probe=argv[++i];else {fprintf(stderr,"usage: %s [--bus PATH] [--probe PATH]\\n",argv[0]);return 2;}}
    int fd=connect_bus(bus);if(fd<0){fprintf(stderr,"display-manager: IPC bus connection failed: %s\\n",strerror(errno));return 1;}
    if(send_line(fd,"{\"type\":\"register\",\"service\":\"display-manager\",\"version\":\"1.0\"}\n")<0){close(fd);return 1;}
    char line[32768];if(read_line(fd,line,sizeof(line))<0){close(fd);return 1;}
    for(;;){
        if(read_line(fd,line,sizeof(line))<0)break;
        char type[64]={0},op[128]={0},rid[256]={0};if(json_string(line,"type",type,sizeof(type))<0)continue;if(strcmp(type,"request"))continue;
        if(json_string(line,"request_id",rid,sizeof(rid))<0)continue;int rc=json_string(line,"operation",op,sizeof(op));
        if(rc<0){send_line(fd,"{\"type\":\"response\",\"request_id\":\"\",\"status\":\"error\",\"payload\":{\"code\":\"INVALID_REQUEST\"}}\n");continue;}
        if(!strcmp(op,"get_capabilities")){char msg[1024];snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"request_id\":\"%s\",\"status\":\"ok\",\"payload\":{\"capabilities\":[\"display.enumerate\",\"display.modes\",\"drm.kms\"]}}\n",rid);send_line(fd,msg);continue;}
        if(!strcmp(op,"get_displays")||!strcmp(op,"probe")){char payload[32768];if(run_probe(probe,payload,sizeof(payload))<0){char msg[1024];snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"request_id\":\"%s\",\"status\":\"error\",\"payload\":{\"code\":\"DISPLAY_PROBE_FAILED\"}}\n",rid);send_line(fd,msg);}else{char msg[49152];snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"request_id\":\"%s\",\"status\":\"ok\",\"payload\":%s}\n",rid,payload);send_line(fd,msg);}continue;}
        char msg[2048];snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"request_id\":\"%s\",\"status\":\"error\",\"payload\":{\"code\":\"UNSUPPORTED_OPERATION\"}}\n",rid);send_line(fd,msg);
    }
    close(fd);return 0;
}
