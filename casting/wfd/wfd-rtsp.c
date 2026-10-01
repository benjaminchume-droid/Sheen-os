#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "sheen/wfd.h"

struct sheen_wfd_server {
    int fd;
    uint16_t port;
    uint16_t rtp_port;
    uint16_t width;
    uint16_t height;
    uint32_t fps;
    pthread_t thread;
    int stop;
};

static int send_all(int fd,const char *s,size_t n) {
    size_t p=0;
    while(p<n) {
        ssize_t w=send(fd,s+p,n-p,0);
        if(w<0) { if(errno==EINTR) continue; return -1; }
        if(w==0) return -1;
        p += (size_t)w;
    }
    return 0;
}

static void response(int fd,const char *cseq,int code,const char *extra,const char *body) {
    char out[8192];
    const char *reason=code==200?"OK":code==400?"Bad Request":"Not Found";
    size_t bl=body?strlen(body):0;
    int n=snprintf(out,sizeof(out),
        "RTSP/1.0 %d %s\r\nCSeq: %s\r\nSession: 1\r\n%s%s%sContent-Length: %zu\r\n\r\n%s",
        code,reason,cseq?cseq:"0",extra?extra:"",body?"Content-Type: text/parameters\r\n":"",
        body?"":"",bl,body?body:"");
    if(n>0 && (size_t)n<sizeof(out)) send_all(fd,out,(size_t)n);
}

static void handle_client(int fd,sheen_wfd_server *s) {
    char req[16384];
    size_t used=0;
    while(!s->stop && used+1<sizeof(req)) {
        ssize_t n=recv(fd,req+used,sizeof(req)-used-1,0);
        if(n<0) { if(errno==EINTR) continue; break; }
        if(n==0) break;
        used += (size_t)n;
        req[used]=0;
        if(!strstr(req,"\r\n\r\n")) continue;

        char method[32]={0},uri[2048]={0},cseq[64]="0";
        sscanf(req,"%31s %2047s",method,uri);
        const char *cp=strcasestr(req,"CSeq:");
        if(cp) sscanf(cp+5," %63[^\r\n]",cseq);

        if(!strcmp(method,"OPTIONS")) {
            const char extra[]="Public: OPTIONS, GET_PARAMETER, SET_PARAMETER, SETUP, PLAY, TEARDOWN\r\n";
            response(fd,cseq,200,extra,NULL);
        } else if(!strcmp(method,"GET_PARAMETER")) {
            char body[1024];
            snprintf(body,sizeof(body),
                "wfd_video_formats: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00\r\n"
                "wfd_video3d_formats: none\r\n"
                "wfd_audio_codecs: AAC 00000001 0 0\r\n"
                "wfd_content_protection: none\r\n"
                "wfd_display_edid: none\r\n"
                "wfd_connector_type: 5\r\n"
                "wfd_client_rtp_ports: RTP/AVP/UDP;unicast %u 0 mode=play\r\n",
                s->rtp_port);
            response(fd,cseq,200,"",body);
        } else if(!strcmp(method,"SET_PARAMETER")) {
            response(fd,cseq,200,"",NULL);
        } else if(!strcmp(method,"SETUP")) {
            char extra[512];
            snprintf(extra,sizeof(extra),
                "Transport: RTP/AVP/UDP;unicast;client_port=%u-%u;server_port=%u-%u\r\n",
                s->rtp_port,s->rtp_port+1,s->rtp_port,s->rtp_port+1);
            response(fd,cseq,200,extra,NULL);
        } else if(!strcmp(method,"PLAY")) {
            response(fd,cseq,200,"Range: npt=0.000-\r\n",NULL);
        } else if(!strcmp(method,"TEARDOWN")) {
            response(fd,cseq,200,"",NULL);
            break;
        } else {
            response(fd,cseq,400,"",NULL);
        }
        used=0;
    }
}

static void *server_thread(void *arg) {
    sheen_wfd_server *s=arg;
    while(!s->stop) {
        struct sockaddr_in peer={0};
        socklen_t len=sizeof(peer);
        int c=accept(s->fd,(struct sockaddr *)&peer,&len);
        if(c<0) { if(errno==EINTR) continue; if(s->stop) break; continue; }
        handle_client(c,s);
        close(c);
    }
    return NULL;
}

sheen_wfd_server *sheen_wfd_start(const sheen_wfd_config *cfg) {
    if(!cfg || !cfg->rtsp_port || !cfg->rtp_port || !cfg->width || !cfg->height || !cfg->fps) return NULL;
    sheen_wfd_server *s=calloc(1,sizeof(*s));
    if(!s) return NULL;
    s->fd=socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC,0);
    if(s->fd<0) { free(s); return NULL; }
    int one=1;
    setsockopt(s->fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
    struct sockaddr_in addr={0};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(cfg->rtsp_port);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(s->fd,(struct sockaddr *)&addr,sizeof(addr))<0 || listen(s->fd,8)<0) {
        close(s->fd); free(s); return NULL;
    }
    s->port=cfg->rtsp_port;
    s->rtp_port=cfg->rtp_port;
    s->width=cfg->width;
    s->height=cfg->height;
    s->fps=cfg->fps;
    if(pthread_create(&s->thread,NULL,server_thread,s)!=0) {
        close(s->fd); free(s); return NULL;
    }
    return s;
}

void sheen_wfd_stop(sheen_wfd_server *s) {
    if(!s) return;
    s->stop=1;
    shutdown(s->fd,SHUT_RDWR);
    close(s->fd);
    pthread_join(s->thread,NULL);
    free(s);
}

int sheen_wfd_port(const sheen_wfd_server *s) {
    return s ? (int)s->port : -1;
}
