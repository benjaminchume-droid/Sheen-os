#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "sheen/discovery.h"
static int make_udp(int port,int reuse){int fd=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,0);if(fd<0)return -1;if(reuse){int one=1;setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));}if(port){struct sockaddr_in a={0};a.sin_family=AF_INET;a.sin_port=htons((uint16_t)port);a.sin_addr.s_addr=htonl(INADDR_ANY);if(bind(fd,(struct sockaddr*)&a,sizeof(a))<0){close(fd);return -1;}}return fd;}
static int send_ssdp(int fd){static const char req[]="M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nMAN:\"ssdp:discover\"\r\nMX:2\r\nST:ssdp:all\r\n\r\n";struct sockaddr_in a={0};a.sin_family=AF_INET;a.sin_port=htons(1900);inet_pton(AF_INET,"239.255.255.250",&a.sin_addr);return sendto(fd,req,sizeof(req)-1,0,(struct sockaddr*)&a,sizeof(a))<0?-1:0;}
static const char *header_value(const char *msg,const char *header,char *out,size_t n){size_t hn=strlen(header);const char *p=msg;while((p=strstr(p,"\r\n"))){p+=2;if(!strncasecmp(p,header,hn)&&p[hn]==':'){p+=hn+1;while(*p==' '||*p=='\t')p++;const char *e=strstr(p,"\r\n");size_t len=e?(size_t)(e-p):strlen(p);if(len>=n)len=n-1;memcpy(out,p,len);out[len]=0;return out;}}return NULL;}
static int add_ssdp(sheen_cast_discovery_result *r,const char *msg,const char *addr){char loc[512]={0},st[256]={0},usn[512]={0},server[256]={0};header_value(msg,"LOCATION",loc,sizeof(loc));header_value(msg,"ST",st,sizeof(st));header_value(msg,"USN",usn,sizeof(usn));header_value(msg,"SERVER",server,sizeof(server));if(!usn[0]&&!loc[0])return 0;for(size_t i=0;i<r->count;i++)if(!strcmp(r->devices[i].device_id,usn[0]?usn:loc))return 0;if(r->count>=SHEEN_CAST_MAX_DEVICES)return ENOSPC;sheen_cast_device *d=&r->devices[r->count++];snprintf(d->device_id,sizeof(d->device_id),"%s",usn[0]?usn:loc);snprintf(d->name,sizeof(d->name),"%s",server[0]?server:(st[0]?st:"UPnP/SSDP device"));snprintf(d->address,sizeof(d->address),"%s",addr);d->port=0;snprintf(d->capabilities,sizeof(d->capabilities),"ssdp");return 0;}
int sheen_cast_discover(uint32_t timeout_ms,sheen_cast_discovery_result *r){if(!r||timeout_ms==0)return EINVAL;memset(r,0,sizeof(*r));int fd=make_udp(0,0);if(fd<0)return errno;if(send_ssdp(fd)<0){int e=errno;close(fd);return e;}struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);int64_t deadline=(int64_t)ts.tv_sec*1000+ts.tv_nsec/1000000+timeout_ms;char buf[8192];while(1){clock_gettime(CLOCK_MONOTONIC,&ts);int64_t now=(int64_t)ts.tv_sec*1000+ts.tv_nsec/1000000;if(now>=deadline)break;struct pollfd p={fd,POLLIN,0};int left=(int)(deadline-now);if(left<1)left=1;if(poll(&p,1,left)<=0)continue;if(!(p.revents&POLLIN))continue;struct sockaddr_in from={0};socklen_t fl=sizeof(from);ssize_t n=recvfrom(fd,buf,sizeof(buf)-1,0,(struct sockaddr*)&from,&fl);if(n<=0)continue;buf[n]=0;char addr[INET_ADDRSTRLEN];if(!inet_ntop(AF_INET,&from.sin_addr,addr,sizeof(addr)))continue;add_ssdp(r,buf,addr);}close(fd);return 0;}
