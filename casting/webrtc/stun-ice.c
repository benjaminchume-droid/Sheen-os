#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sheen/webrtc.h"

struct sheen_webrtc_ice { int fd; uint16_t port; };

static uint16_t be16(const uint8_t *p){ return (uint16_t)(((uint16_t)p[0]<<8)|p[1]); }
static uint32_t be32(const uint8_t *p){ return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; }
static void put16(uint8_t *p,uint16_t v){p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v;}
static void put32(uint8_t *p,uint32_t v){p[0]=(uint8_t)(v>>24);p[1]=(uint8_t)(v>>16);p[2]=(uint8_t)(v>>8);p[3]=(uint8_t)v;}

sheen_webrtc_ice *sheen_webrtc_ice_open(uint16_t port){
    int fd=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,0);if(fd<0)return NULL;
    int one=1;setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
    struct sockaddr_in a={0};a.sin_family=AF_INET;a.sin_port=htons(port);a.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(fd,(struct sockaddr*)&a,sizeof(a))<0){close(fd);return NULL;}
    sheen_webrtc_ice *ice=calloc(1,sizeof(*ice));if(!ice){close(fd);return NULL;}ice->fd=fd;ice->port=port;return ice;
}

int sheen_webrtc_ice_process(sheen_webrtc_ice *ice){
    if(!ice)return EINVAL;
    uint8_t in[2048];
    struct sockaddr_in peer={0};socklen_t plen=sizeof(peer);
    ssize_t n=recvfrom(ice->fd,in,sizeof(in),0,(struct sockaddr*)&peer,&plen);
    if(n<20)return n<0?errno:EPROTO;
    if((in[0]&0xC0)!=0 || in[0]!=0x00 || in[1]!=0x01)return ENOTSUP;
    uint16_t msg_len=be16(in+2);if(msg_len+20>(size_t)n)return EPROTO;
    uint32_t cookie=be32(in+4);if(cookie!=0x2112A442)return EPROTO;
    uint8_t out[256]={0};
    put16(out,0x0101);
    put16(out+2,12);
    put32(out+4,cookie);
    memcpy(out+8,in+8,12);
    put16(out+20,0x0020);
    put16(out+22,8);
    uint32_t addr=((uint32_t)peer.sin_addr.s_addr)^cookie;
    uint16_t port=(uint16_t)(ntohs(peer.sin_port)^((cookie>>16)&0xffff));
    put16(out+24,(uint16_t)(0x0001|0x0000));
    out[26]=0;out[27]=1;
    put16(out+28,port^0);
    put32(out+30,addr);
    return (int)sendto(ice->fd,out,38,0,(struct sockaddr*)&peer,plen);
}
int sheen_webrtc_ice_fd(const sheen_webrtc_ice *ice){return ice?ice->fd:-1;}
int sheen_webrtc_ice_port(const sheen_webrtc_ice *ice){return ice?ice->port:-1;}
void sheen_webrtc_ice_close(sheen_webrtc_ice *ice){if(!ice)return;close(ice->fd);free(ice);}
