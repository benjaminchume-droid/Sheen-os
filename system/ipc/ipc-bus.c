#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define MAX_PEERS 64
#define MAX_PENDING 256
#define INPUT_SIZE 16384
#define ID_SIZE 256
#define SERVICE_SIZE 128
#define VERSION_SIZE 32
#define SOCKET_SIZE 108
typedef struct { int fd; int role; char service[SERVICE_SIZE]; char version[VERSION_SIZE]; char in[INPUT_SIZE]; size_t in_len; } peer_t;
typedef struct { int used; char request_id[ID_SIZE]; int client_fd; int service_fd; int64_t deadline_ms; } pending_t;
static volatile sig_atomic_t stopping=0; static peer_t peers[MAX_PEERS]; static pending_t pending[MAX_PENDING];
static void on_signal(int sig){(void)sig;stopping=1;}
static int64_t mono_ms(void){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (int64_t)ts.tv_sec*1000+(ts.tv_nsec/1000000);}
static int mkdir_p(const char *path){char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return -1;memcpy(b,path,n+1);for(size_t i=1;i<n;i++)if(b[i]=='/'){b[i]=0;if(mkdir(b,0755)<0&&errno!=EEXIST)return -1;b[i]='/';}if(mkdir(b,0755)<0&&errno!=EEXIST)return -1;return 0;}
static void json_escape(const char *s,char *out,size_t n){size_t p=0;if(n)p++;if(n)out[0]=34;for(size_t i=0;s&&s[i]&&p+2<n;i++){unsigned char c=s[i];if(c==34||c==92){out[p++]=92;out[p++]=(char)c;}else if(c<32){out[p++]=32;}else out[p++]=(char)c;}if(p<n)out[p++]=34;if(p<n)out[p]=0;else if(n)out[n-1]=0;}
static int json_string(const char *line,const char *key,char *out,size_t n){char pat[160];int m=snprintf(pat,sizeof(pat),"\\"%s\\":\\"",key);if(m<0||(size_t)m>=sizeof(pat)||!out||n<2)return -1;const char *p=strstr(line,pat);if(!p)return -1;p+=m;size_t w=0;while(*p&&w+1<n){if(*p==92&&p[1]){char c=p[1];if(c==34||c==92||c==47){out[w++]=c;p+=2;continue;}if(c=='n'){out[w++]=10;p+=2;continue;}if(c=='r'){out[w++]=13;p+=2;continue;}if(c=='t'){out[w++]=9;p+=2;continue;}}if(*p==34){out[w]=0;return 0;}out[w++]=*p++;}return -1;}
static int send_line(int fd,const char *line){size_t n=strlen(line),p=0;while(p<n){ssize_t w=send(fd,line+p,n-p,MSG_NOSIGNAL);if(w<0){if(errno==EINTR)continue;return -1;}p+=(size_t)w;}return 0;}
static peer_t *peer_for_fd(int fd){for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd==fd)return &peers[i];return NULL;}
static void remove_peer(int fd){for(size_t i=0;i<MAX_PENDING;i++)if(pending[i].used&&(pending[i].client_fd==fd||pending[i].service_fd==fd))pending[i].used=0;for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd==fd){close(fd);peers[i].fd=-1;peers[i].role=0;peers[i].service[0]=0;break;}}
static int find_service(const char *name){for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0&&peers[i].role==2&&!strcmp(peers[i].service,name))return peers[i].fd;return -1;}
static void respond(int fd,const char *request_id,const char *status,const char *payload){char id[ID_SIZE*2],msg[8192];json_escape(request_id,id,sizeof(id));if(!payload)payload="{}";snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"request_id\":%s,\"status\":\"%s\",\"payload\":%s}\n",id,status,payload);send_line(fd,msg);}
static int add_pending(const char *id,int client,int service,int64_t deadline){for(size_t i=0;i<MAX_PENDING;i++)if(!pending[i].used){pending[i].used=1;snprintf(pending[i].request_id,sizeof(pending[i].request_id),"%s",id);pending[i].client_fd=client;pending[i].service_fd=service;pending[i].deadline_ms=deadline;return 0;}return -1;}
static pending_t *take_pending(const char *id){for(size_t i=0;i<MAX_PENDING;i++)if(pending[i].used&&!strcmp(pending[i].request_id,id))return &pending[i];return NULL;}
static int64_t field_deadline(const char *line){const char *p=strstr(line,"\"deadline_ms\"");if(!p)return 0;p=strchr(p,':');if(!p)return 0;while(*++p==' '||*p=='\t');char *e=NULL;long long v=strtoll(p,&e,10);return e==p?0:v;}
static void broadcast_event(const char *line,int except_fd){for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0&&peers[i].role==1&&peers[i].fd!=except_fd)send_line(peers[i].fd,line);}
static void handle_message(peer_t *p,const char *line){
    char type[64]={0},service[SERVICE_SIZE]={0},version[VERSION_SIZE]={0},operation[128]={0},request_id[ID_SIZE]={0};
    if(json_string(line,"type",type,sizeof(type))<0)return;
    if(!strcmp(type,"register")){
        if(json_string(line,"service",service,sizeof(service))<0||json_string(line,"version",version,sizeof(version))<0){send_line(p->fd,"{\"type\":\"error\",\"code\":\"INVALID_REGISTER\"}\n");return;}
        p->role=2;snprintf(p->service,sizeof(p->service),"%s",service);snprintf(p->version,sizeof(p->version),"%s",version);
        send_line(p->fd,"{\"type\":\"registered\",\"status\":\"ok\"}\n");return;
    }
    if(!strcmp(type,"discover")){
        int filter=json_string(line,"service",service,sizeof(service))==0; char msg[8192];size_t n=0; n+=(size_t)snprintf(msg+n,sizeof(msg)-n,"{\"type\":\"response\",\"status\":\"ok\",\"payload\":[");int first=1;
        for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0&&peers[i].role==2&&(!filter||!strcmp(service,peers[i].service))){char es[SERVICE_SIZE*2],ev[VERSION_SIZE*2];json_escape(peers[i].service,es,sizeof(es));json_escape(peers[i].version,ev,sizeof(ev));n+=(size_t)snprintf(msg+n,sizeof(msg)-n,"%s{\"service\":%s,\"versions\":[%s],\"capabilities\":[]}",first?"":",",es,ev);first=0;}
        snprintf(msg+n,sizeof(msg)-n,"]}\n");send_line(p->fd,msg);return;
    }
    if(!strcmp(type,"negotiate")){
        if(json_string(line,"service",service,sizeof(service))<0||json_string(line,"version",version,sizeof(version))<0){respond(p->fd,"","error","{\"code\":\"INVALID_NEGOTIATE\"}");return;}
        int sf=find_service(service);if(sf<0){respond(p->fd,"","error","{\"code\":\"SERVICE_UNAVAILABLE\"}");return;}
        peer_t *sp=peer_for_fd(sf);char av[VERSION_SIZE*2],msg[2048];json_escape(sp->version,av,sizeof(av));snprintf(msg,sizeof(msg),"{\"type\":\"response\",\"status\":\"ok\",\"payload\":{\"accepted_version\":%s,\"capabilities\":[]}}\n",av);send_line(p->fd,msg);return;
    }
    if(!strcmp(type,"request")){
        if(json_string(line,"service",service,sizeof(service))<0||json_string(line,"operation",operation,sizeof(operation))<0||json_string(line,"request_id",request_id,sizeof(request_id))<0){respond(p->fd,request_id,"error","{\"code\":\"INVALID_REQUEST\"}");return;}
        int sf=find_service(service);if(sf<0){respond(p->fd,request_id,"error","{\"code\":\"SERVICE_UNAVAILABLE\"}");return;}
        int64_t dl=field_deadline(line);int64_t exp=dl?mono_ms()+dl:0;if(add_pending(request_id,p->fd,sf,exp)<0){respond(p->fd,request_id,"error","{\"code\":\"BUS_FULL\"}");return;}
        if(send_line(sf,line)<0){pending_t *q=take_pending(request_id);if(q)q->used=0;respond(p->fd,request_id,"error","{\"code\":\"SERVICE_WRITE_FAILED\"}");}return;
    }
    if(!strcmp(type,"response")){
        if(json_string(line,"request_id",request_id,sizeof(request_id))<0)return;pending_t *q=take_pending(request_id);if(!q)return;send_line(q->client_fd,line);q->used=0;return;
    }
    if(!strcmp(type,"event")){broadcast_event(line,p->fd);return;}
}

static int setup_socket(const char *path){
    char dir[4096];snprintf(dir,sizeof(dir),"%s",path);char *slash=strrchr(dir,'/');if(slash){*slash=0;mkdir_p(dir);}
    unlink(path);int s=socket(AF_UNIX,SOCK_STREAM,0);if(s<0)return -1;struct sockaddr_un a;memset(&a,0,sizeof(a));a.sun_family=AF_UNIX;if(strlen(path)>=sizeof(a.sun_path)){close(s);return -1;}strcpy(a.sun_path,path);
    if(bind(s,(struct sockaddr*)&a,sizeof(a))<0||chmod(path,0660)<0||listen(s,32)<0){close(s);unlink(path);return -1;}return s;
}

int main(int argc,char **argv){
    const char *socket_path="/run/sheen/ipc/bus.sock";for(int i=1;i<argc;i++){if(!strcmp(argv[i],"--socket")&&i+1<argc)socket_path=argv[++i];else {fprintf(stderr,"usage: %s [--socket PATH]\\n",argv[0]);return 2;}}
    struct sigaction sa;memset(&sa,0,sizeof(sa));sa.sa_handler=on_signal;sigemptyset(&sa.sa_mask);sigaction(SIGTERM,&sa,NULL);sigaction(SIGINT,&sa,NULL);
    int listener=setup_socket(socket_path);if(listener<0){fprintf(stderr,"sheen-ipc-bus: socket setup failed: %s\\n",strerror(errno));return 1;}
    for(size_t i=0;i<MAX_PEERS;i++)peers[i].fd=-1;
    while(!stopping){
        struct pollfd pf[MAX_PEERS+1];size_t count=1;pf[0].fd=listener;pf[0].events=POLLIN;
        for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0){pf[count].fd=peers[i].fd;pf[count].events=POLLIN;count++;}
        int pr=poll(pf,count,250);if(pr<0){if(errno==EINTR)continue;break;}
        for(size_t i=0;i<MAX_PENDING;i++)if(pending[i].used&&pending[i].deadline_ms>0&&mono_ms()>=pending[i].deadline_ms){char rid[ID_SIZE];snprintf(rid,sizeof(rid),"%s",pending[i].request_id);respond(pending[i].client_fd,rid,"deadline_exceeded","{}");pending[i].used=0;}
        size_t idx=1;for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0){int fd=peers[i].fd;short rev=pf[idx++].revents;if(rev&(POLLERR|POLLHUP|POLLNVAL)){remove_peer(fd);continue;}if(!(rev&POLLIN))continue;peer_t *p=peer_for_fd(fd);if(!p)continue;ssize_t r=recv(fd,p->in+p->in_len,sizeof(p->in)-p->in_len-1,0);if(r<=0){remove_peer(fd);continue;}p->in_len+=(size_t)r;p->in[p->in_len]=0;size_t start=0;for(size_t j=0;j<p->in_len;j++)if(p->in[j]==10){p->in[j]=0;if(j>start)handle_message(p,p->in+start);start=j+1;}if(start){size_t rem=p->in_len-start;memmove(p->in,p->in+start,rem);p->in_len=rem;}}
        if(pr>0&&(pf[0].revents&POLLIN)){int fd=accept(listener,NULL,NULL);if(fd>=0){size_t slot=MAX_PEERS;for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd<0){slot=i;break;}if(slot<MAX_PEERS){peers[slot].fd=fd;peers[slot].role=1;}else close(fd);}}
    }
    for(size_t i=0;i<MAX_PEERS;i++)if(peers[i].fd>=0)close(peers[i].fd);close(listener);unlink(socket_path);return 0;
}
