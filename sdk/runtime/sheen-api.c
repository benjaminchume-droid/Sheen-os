#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include "sheen/api.h"

struct sheen_api_client {
    int fd;
    uint32_t timeout_ms;
    uint64_t sequence;
    char buffer[131072];
    size_t buffer_len;
};

static uint64_t now_ms(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return (uint64_t)t.tv_sec*1000ULL+(uint64_t)t.tv_nsec/1000000ULL;
}
static int send_all(int fd,const char *data,size_t n){
    size_t p=0;
    while(p<n){
        ssize_t w=send(fd,data+p,n-p,MSG_NOSIGNAL);
        if(w<0){if(errno==EINTR)continue;return errno;}
        if(!w)return EIO;
        p+=(size_t)w;
    }
    return 0;
}
static int next_line(sheen_api_client *c,char *out,size_t cap){
    for(;;){
        char *nl=memchr(c->buffer,'\n',c->buffer_len);
        if(nl){
            size_t n=(size_t)(nl-c->buffer)+1;
            if(n>cap)return EMSGSIZE;
            memcpy(out,c->buffer,n-1);out[n-1]=0;
            size_t rem=c->buffer_len-n;
            memmove(c->buffer,c->buffer+n,rem);
            c->buffer_len=rem;
            return 0;
        }
        if(c->buffer_len==sizeof(c->buffer))return EMSGSIZE;
        ssize_t r=recv(c->fd,c->buffer+c->buffer_len,sizeof(c->buffer)-c->buffer_len,0);
        if(r<0){if(errno==EINTR)continue;return errno;}
        if(!r)return ECONNRESET;
        c->buffer_len+=(size_t)r;
    }
}
static int json_string(const char *line,const char *key,char *out,size_t cap){
    char pat[256];
    int n=snprintf(pat,sizeof(pat),"\"%s\":\"",key);
    if(n<0||(size_t)n>=sizeof(pat)||cap<1)return EINVAL;
    const char *p=strstr(line,pat);
    if(!p)return ENOENT;
    p+=n;
    size_t w=0;
    while(*p && w+1<cap){
        if(*p==92 && p[1]){
            if(p[1]==34||p[1]==92||p[1]==47){out[w++]=p[1];p+=2;continue;}
            if(p[1]=='n'){out[w++]=10;p+=2;continue;}
            if(p[1]=='r'){out[w++]=13;p+=2;continue;}
            if(p[1]=='t'){out[w++]=9;p+=2;continue;}
        }
        if(*p==34){out[w]=0;return 0;}
        out[w++]=*p++;
    }
    return EMSGSIZE;
}
static int connect_unix(const char *path){
    int fd=socket(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0);
    if(fd<0)return -1;
    struct sockaddr_un a={0};
    a.sun_family=AF_UNIX;
    if(strlen(path)>=sizeof(a.sun_path)){close(fd);return -1;}
    snprintf(a.sun_path,sizeof(a.sun_path),"%s",path);
    if(connect(fd,(struct sockaddr*)&a,sizeof(a))<0){close(fd);return -1;}
    return fd;
}
sheen_api_client *sheen_api_connect(const char *bus,uint32_t timeout_ms){
    if(!bus||!*bus)return NULL;
    int fd=connect_unix(bus);
    if(fd<0)return NULL;
    sheen_api_client *c=calloc(1,sizeof(*c));
    if(!c){close(fd);return NULL;}
    c->fd=fd;c->timeout_ms=timeout_ms?timeout_ms:5000;c->sequence=0;
    return c;
}
int sheen_api_request(sheen_api_client *c,const char *service,const char *operation,const char *payload,sheen_api_response *r){
    if(!c||!service||!operation||!r)return EINVAL;
    if(!payload)payload="{}";
    char req_id[64],line[70000];
    snprintf(req_id,sizeof(req_id),"%llu",(unsigned long long)++c->sequence);
    int n=snprintf(line,sizeof(line),
        "{\"type\":\"request\",\"service\":\"%s\",\"operation\":\"%s\",\"request_id\":\"%s\",\"payload\":%s,\"deadline_ms\":%u}\n",
        service,operation,req_id,payload,c->timeout_ms);
    if(n<0||(size_t)n>=sizeof(line))return EMSGSIZE;
    int rc=send_all(c->fd,line,(size_t)n);if(rc)return rc;
    uint64_t deadline=now_ms()+c->timeout_ms;
    for(;;){
        int remain=(int)(deadline>now_ms()?deadline-now_ms():0);
        struct pollfd p={c->fd,POLLIN,0};
        int pr=poll(&p,1,remain);
        if(pr<0){if(errno==EINTR)continue;return errno;}
        if(pr==0)return ETIMEDOUT;
        if(!(p.revents&POLLIN))return EIO;
        char response[70000];
        rc=next_line(c,response,sizeof(response));if(rc)return rc;
        char type[32]={0},rid[128]={0};
        if(json_string(response,"type",type,sizeof(type))<0)continue;
        if(!strcmp(type,"event")){continue;}
        if(json_string(response,"request_id",rid,sizeof(rid))<0)continue;
        if(strcmp(rid,req_id))continue;
        memset(r,0,sizeof(*r));
        json_string(response,"status",r->status,sizeof(r->status));
        snprintf(r->request_id,sizeof(r->request_id),"%s",rid);
        const char *p_payload=strstr(response,"\"payload\":");
        if(p_payload)snprintf(r->payload,sizeof(r->payload),"%s",p_payload+10);
        if(r->status[0]&&!strcmp(r->status,"ok"))return 0;
        return EIO;
    }
}
int sheen_api_subscribe(sheen_api_client *c,const char *service,const char *event){
    if(!c||!service||!event)return EINVAL;
    char line[1024];
    int n=snprintf(line,sizeof(line),
        "{\"type\":\"subscribe\",\"service\":\"%s\",\"event\":\"%s\"}\n",service,event);
    if(n<0||(size_t)n>=sizeof(line))return EMSGSIZE;
    return send_all(c->fd,line,(size_t)n);
}
int sheen_api_next_event(sheen_api_client *c,char *out,size_t cap){
    if(!c||!out||cap<2)return EINVAL;
    uint64_t deadline=now_ms()+c->timeout_ms;
    for(;;){
        int remain=(int)(deadline>now_ms()?deadline-now_ms():0);
        struct pollfd p={c->fd,POLLIN,0};
        int pr=poll(&p,1,remain);
        if(pr<0){if(errno==EINTR)continue;return errno;}
        if(!pr)return ETIMEDOUT;
        char line[65536];int rc=next_line(c,line,sizeof(line));if(rc)return rc;
        char type[32]={0};if(json_string(line,"type",type,sizeof(type))<0)continue;
        if(strcmp(type,"event"))continue;
        size_t n=strlen(line);if(n>=cap)return EMSGSIZE;
        memcpy(out,line,n+1);return 0;
    }
}
void sheen_api_close(sheen_api_client *c){if(!c)return;close(c->fd);free(c);}
const char *sheen_api_transport_name(void){return "unix-domain-jsonl-v1";}
