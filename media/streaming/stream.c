#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sheen/stream.h"

struct sheen_stream { sheen_stream_type type; int fd; int eof; uint64_t remaining; int has_length; char uri[4096]; unsigned char prefix[16384]; size_t prefix_len; size_t prefix_pos; };
static int parse_host_port(const char *authority,char *host,size_t host_n,char *port,size_t port_n,const char *default_port){
    if(!authority||!host||!port)return -1;const char *colon=strrchr(authority,':');
    if(colon&&strchr(authority,']')==NULL){size_t hn=(size_t)(colon-authority);if(hn==0||hn>=host_n)return -1;memcpy(host,authority,hn);host[hn]=0;snprintf(port,port_n,"%s",colon+1);}
    else {snprintf(host,host_n,"%s",authority);snprintf(port,port_n,"%s",default_port);}
    return 0;
}
static int tcp_connect(const char *host,const char *port){struct addrinfo hints={0},*res=NULL;hints.ai_socktype=SOCK_STREAM;hints.ai_family=AF_UNSPEC;int rc=getaddrinfo(host,port,&hints,&res);if(rc)return -1;int fd=-1;for(struct addrinfo *p=res;p;p=p->ai_next){fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);if(fd<0)continue;if(connect(fd,p->ai_addr,p->ai_addrlen)==0)break;close(fd);fd=-1;}freeaddrinfo(res);return fd;}
static int parse_http_headers(sheen_stream *s,char *headers,size_t n){
    char *line=strtok(headers,"\r\n");if(!line)return -1;int status=0;if(sscanf(line,"HTTP/%*s %d",&status)!=1||status<200||status>=300)return -1;
    s->has_length=0;s->remaining=0;
    while((line=strtok(NULL,"\r\n"))){char *colon=strchr(line,':');if(!colon)continue;*colon=0;char *key=line;char *value=colon+1;while(*value==' '||*value=='\t')value++;
        for(char *p=key;*p;p++)if(*p>='A'&&*p<='Z')*p=(char)(*p-'A'+'a');
        if(!strcmp(key,"content-length")){char *end=NULL;unsigned long long x=strtoull(value,&end,10);if(end==value)return -1;s->remaining=x;s->has_length=1;}
        else if(!strcmp(key,"transfer-encoding")&&strstr(value,"chunked"))return -2;
    } return 0;
}
static int open_http(sheen_stream *s,const char *uri){
    const char *p=strstr(uri,"://");if(!p)return -1;const char *authority=p+3;const char *slash=strchr(authority,'/');size_t al=slash?(size_t)(slash-authority):strlen(authority);if(al>=2048)return -1;
    char auth[2048],host[1024],port[32];memcpy(auth,authority,al);auth[al]=0;if(parse_host_port(auth,host,sizeof(host),port,sizeof(port),"80")<0)return -1;
    int fd=tcp_connect(host,port);if(fd<0)return -1;
    const char *path=slash?slash:"/";char req[8192];int n=snprintf(req,sizeof(req),"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nAccept: */*\r\n\r\n",path,host);if(n<0||(size_t)n>=sizeof(req)){close(fd);return -1;}
    size_t sent=0;while(sent<(size_t)n){ssize_t w=send(fd,req+sent,(size_t)n-sent,MSG_NOSIGNAL);if(w<0){if(errno==EINTR)continue;close(fd);return -1;}if(!w){close(fd);return -1;}sent+=(size_t)w;}
    char headers[16384];size_t got=0;size_t body_start=0;int found=0;while(got+1<sizeof(headers)){ssize_t r=recv(fd,headers+got,sizeof(headers)-got-1,0);if(r<0){if(errno==EINTR)continue;close(fd);return -1;}if(!r)break;got+=(size_t)r;headers[got]=0;char *sep=strstr(headers,"\r\n\r\n");if(sep){found=1;body_start=(size_t)(sep+4-headers);break;}}
    if(!found){close(fd);return -1;} char *copy=malloc(body_start+1);if(!copy){close(fd);return -1;}memcpy(copy,headers,body_start);copy[body_start]=0;
    int rc=parse_http_headers(s,copy,body_start);free(copy);if(rc==-2||rc<0){close(fd);return -1;}
    s->fd=fd;if(body_start<got){size_t body=got-body_start;if(body>sizeof(s->prefix))body=sizeof(s->prefix);memcpy(s->prefix,headers+body_start,body);s->prefix_len=body;s->prefix_pos=0;}
    return 0;
}
static int open_udp(sheen_stream *s,const char *uri){
    const char *a=strstr(uri,"://");if(!a)return -1;a+=3;char auth[256];snprintf(auth,sizeof(auth),"%s",a);char *slash=strchr(auth,'/');if(slash)*slash=0;
    char host[128],port[32];if(parse_host_port(auth,host,sizeof(host),port,sizeof(port),"0")<0)return -1;struct addrinfo hints={0},*res=NULL;hints.ai_socktype=SOCK_DGRAM;hints.ai_family=AF_INET;hints.ai_flags=AI_PASSIVE;int rc=getaddrinfo(host,*port?port:"0",&hints,&res);if(rc)return -1;
    int fd=socket(AF_INET,SOCK_DGRAM,0);if(fd<0){freeaddrinfo(res);return -1;}struct sockaddr_in local={0};local.sin_family=AF_INET;local.sin_port=((struct sockaddr_in*)res->ai_addr)->sin_port;local.sin_addr.s_addr=htonl(INADDR_ANY);if(bind(fd,(struct sockaddr*)&local,sizeof(local))<0){close(fd);freeaddrinfo(res);return -1;}
    struct in_addr group=((struct sockaddr_in*)res->ai_addr)->sin_addr;if((ntohl(group.s_addr)&0xf0000000U)==0xe0000000U){struct ip_mreq m={.imr_multiaddr=group,.imr_interface={0}};setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&m,sizeof(m));}
    freeaddrinfo(res);s->fd=fd;return 0;
}
sheen_stream *sheen_stream_open(const char *uri){if(!uri||!*uri)return NULL;sheen_stream *s=calloc(1,sizeof(*s));if(!s)return NULL;s->fd=-1;snprintf(s->uri,sizeof(s->uri),"%s",uri);if(!strncmp(uri,"file://",7)||uri[0]=='/'){s->type=SHEEN_STREAM_FILE;const char *p=!strncmp(uri,"file://",7)?uri+7:uri;s->fd=open(p,O_RDONLY|O_CLOEXEC);if(s->fd<0){free(s);return NULL;}return s;}if(!strncmp(uri,"http://",7)){s->type=SHEEN_STREAM_HTTP;if(open_http(s,uri)==0)return s;}else if(!strncmp(uri,"udp://",6)){s->type=SHEEN_STREAM_UDP;if(open_udp(s,uri)==0)return s;}free(s);return NULL;}
ssize_t sheen_stream_read(sheen_stream *s,void *buffer,size_t capacity){if(!s||s->fd<0||!buffer||!capacity)return -1;if(s->prefix_pos<s->prefix_len){size_t n=s->prefix_len-s->prefix_pos;if(n>capacity)n=capacity;if(s->has_length&&n>s->remaining)n=(size_t)s->remaining;memcpy(buffer,s->prefix+s->prefix_pos,n);s->prefix_pos+=n;if(s->has_length){s->remaining-=(uint64_t)n;if(s->remaining==0)s->eof=1;}return (ssize_t)n;}for(;;){ssize_t r;if(s->type==SHEEN_STREAM_FILE)r=read(s->fd,buffer,capacity);else r=recv(s->fd,buffer,capacity,0);if(r<0&&errno==EINTR)continue;if(r<0)return -1;if(r==0){s->eof=1;return 0;}if(s->has_length){if((uint64_t)r>s->remaining)r=(ssize_t)s->remaining;s->remaining-=(uint64_t)r;if(s->remaining==0)s->eof=1;}return r;}}
int sheen_stream_eof(const sheen_stream *s){return s?s->eof:1;}
void sheen_stream_close(sheen_stream *s){if(!s)return;if(s->fd>=0)close(s->fd);free(s);}
sheen_stream_type sheen_stream_type_get(const sheen_stream *s){return s?s->type:SHEEN_STREAM_FILE;}
const char *sheen_stream_uri(const sheen_stream *s){return s?s->uri:"";}
