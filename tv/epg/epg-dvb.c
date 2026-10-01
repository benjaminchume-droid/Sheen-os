#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/epg.h"
static uint8_t bcd(uint8_t v){return (uint8_t)(((v>>4)*10U)+(v&0x0FU));}
static int64_t days_from_civil(int y,unsigned m,unsigned d){y-=m<=2;int era=(y>=0?y:y-399)/400;unsigned yoe=(unsigned)(y-era*400);unsigned doy=(153*(m+(m>2?-3:9))+2)/5+d-1;unsigned doe=yoe*365+yoe/4-yoe/100+doy;return (int64_t)era*146097+(int64_t)doe-719468;}
static void civil_from_days(int64_t z,int *y,unsigned *m,unsigned *d){z+=719468;int era=(z>=0?z:z-146096)/146097;unsigned doe=(unsigned)(z-era*146097);unsigned yoe=(doe-doe/1460+doe/36524-doe/146096)/365;int yy=(int)yoe+era*400;unsigned doy=doe-(365*yoe+yoe/4-yoe/100);unsigned mp=(5*doy+2)/153;*d=doy-(153*mp+2)/5+1;*m=mp+(mp<10?3:-9);if(*m<=2)yy++;*y=yy;}
static int64_t dvb_time_ms(const uint8_t *p){uint16_t mjd=(uint16_t)(((uint16_t)p[0]<<8)|p[1]);int64_t epoch=days_from_civil(1858,11,17);int64_t days=epoch+(int64_t)mjd;int y=0;unsigned m=0,d=0;civil_from_days(days,&y,&m,&d);return ((days_from_civil(y,m,d)*86400LL)+(int64_t)bcd(p[2])*3600LL+(int64_t)bcd(p[3])*60LL+bcd(p[4]))*1000LL;}
static void copy_text(char *dst,size_t cap,const uint8_t *src,size_t len){if(!cap)return;while(len&&(*src==0x00||*src==0x10||*src==0x15)){src++;len--;}if(len>=cap)len=cap-1;memcpy(dst,src,len);dst[len]=0;}
static void parse_short_event(const uint8_t *d,size_t len,sheen_epg_event *e){if(len<6)return;size_t p=3;if(p>=len)return;uint8_t nl=d[p++];if(p+nl>len)return;copy_text(e->title,sizeof(e->title),d+p,nl);p+=nl;if(p>=len)return;uint8_t tl=d[p++];if(p+tl>len)return;copy_text(e->description,sizeof(e->description),d+p,tl);}
static int parse_section(const uint8_t *s,size_t n,uint16_t filter,sheen_epg_document *d){
 if(n<18)return EINVAL;uint8_t tid=s[0];if(tid<0x4E||tid>0x6F)return ENOTSUP;size_t sl=((size_t)(s[1]&0x0F)<<8)|s[2];if(sl+3>n)return EINVAL;uint16_t service=(uint16_t)(((uint16_t)s[3]<<8)|s[4]);if(filter&&service!=filter)return 0;size_t p=14,end=3+sl-4;if(end>n)return EINVAL;
 while(p+12<=end&&d->event_count<SHEEN_EPG_MAX_EVENTS){sheen_epg_event *e=&d->events[d->event_count];memset(e,0,sizeof(*e));e->service_id=service;e->event_id=(uint16_t)(((uint16_t)s[p]<<8)|s[p+1]);e->start_ms=(int64_t)dvb_time_ms(s+p+2);uint32_t dur=((uint32_t)bcd(s[p+7])*3600U+(uint32_t)bcd(s[p+8])*60U+bcd(s[p+9]))*1000U;e->end_ms=e->start_ms+(int64_t)dur;e->running_status=(uint8_t)((s[p+10]>>5)&7U);e->free_ca_mode=(uint8_t)((s[p+10]>>4)&1U);snprintf(e->channel_id,sizeof(e->channel_id),"%u",service);size_t dl=((size_t)(s[p+10]&0x0FU)<<8)|s[p+11];if(p+12+dl>end)return EPROTO;size_t q=p+12,qe=q+dl;while(q+2<=qe){uint8_t tag=s[q],len=s[q+1];if(q+2+len>qe)return EPROTO;if(tag==0x4D)parse_short_event(s+q+2,len,e);q+=2+len;}d->event_count++;p=qe;}return 0;
}
int sheen_epg_parse_dvb_ts(const char *path,uint16_t filter,sheen_epg_document *d){
 if(!path||!d)return EINVAL;memset(d,0,sizeof(*d));FILE *f=fopen(path,"rb");if(!f)return errno;uint8_t pkt[188],sec[8192];size_t len=0,need=0;int collecting=0,rc=0;
 while(fread(pkt,1,sizeof(pkt),f)==sizeof(pkt)){if(pkt[0]!=0x47)continue;uint16_t pid=(uint16_t)(((pkt[1]&0x1FU)<<8)|pkt[2]);if(pid!=0x12)continue;int start=!!(pkt[1]&0x40);int afc=(pkt[3]>>4)&3;size_t off=4;if(afc==0||afc==2)continue;if(afc==3){size_t al=(size_t)pkt[off]+1;if(off+al>=188)continue;off+=al;}if(off>=188)continue;size_t pay=188-off;
  if(start){size_t ptr=pkt[off];if(ptr>=pay)continue;size_t q=off+1+ptr;len=188-q;if(len>sizeof(sec))len=sizeof(sec);memcpy(sec,pkt+q,len);collecting=1;need=0;}else if(collecting){size_t n=pay;if(len+n>sizeof(sec))n=sizeof(sec)-len;memcpy(sec+len,pkt+off,n);len+=n;}
  if(collecting&&len>=3){need=3+(((size_t)(sec[1]&0x0FU)<<8)|sec[2]);if(need>sizeof(sec)){collecting=0;len=0;continue;}if(len>=need){int x=parse_section(sec,need,filter,d);if(x&&x!=ENOTSUP&&rc==0)rc=x;len=0;need=0;collecting=0;}}
  if(d->event_count>=SHEEN_EPG_MAX_EVENTS)break;
 }
 fclose(f);return rc;
}
