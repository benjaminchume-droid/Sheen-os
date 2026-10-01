#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <expat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sheen/epg.h"
typedef struct { sheen_epg_document *doc; sheen_epg_event *current; char active[32]; char text[4096]; size_t text_len; } parser_ctx;
static int64_t days_from_civil(int y,unsigned m,unsigned d){y-=m<=2;int era=(y>=0?y:y-399)/400;unsigned yoe=(unsigned)(y-era*400);unsigned doy=(153*(m+(m>2?-3:9))+2)/5+d-1;unsigned doe=yoe*365+yoe/4-yoe/100+doy;return (int64_t)era*146097+(int64_t)doe-719468;}
static int parse_ts(const char *s,int64_t *out){
    if(!s||!out)return -1;
    int y=0,m=0,d=0,h=0,min=0,sec=0,tzh=0,tzm=0,sign=1;
    const char *tz=NULL;
    int n=sscanf(s,"%4d%2d%2d%2d%2d%2d",&y,&m,&d,&h,&min,&sec);
    if(n<6){
        n=sscanf(s,"%4d-%2d-%2dT%2d:%2d:%2d",&y,&m,&d,&h,&min,&sec);
        if(n<6)return -1;
    }
    tz=strchr(s,'Z');
    if(!tz){
        const char *plus=strrchr(s,'+');const char *minus=strrchr(s,'-');const char *off=plus>minus?plus:minus;
        if(off && off>s+8){sign=*off=='-'?-1:1;if(sscanf(off+1,"%2d%2d",&tzh,&tzm)<1){tzh=0;tzm=0;}}
    }
    int64_t days=days_from_civil(y,(unsigned)m,(unsigned)d);
    *out=((days*86400LL)+(h*3600)+(min*60)+sec)*1000LL-sign*((tzh*60+tzm)*60000LL);
    return 0;
}
static void XMLCALL start(void *ud,const char *name,const char **attrs){parser_ctx *p=ud;p->text_len=0;p->text[0]=0;if(!strcmp(name,"programme")&&p->doc->event_count<SHEEN_EPG_MAX_EVENTS){p->current=&p->doc->events[p->doc->event_count];memset(p->current,0,sizeof(*p->current));for(size_t i=0;attrs&&attrs[i];i+=2){if(!strcmp(attrs[i],"channel"))snprintf(p->current->channel_id,sizeof(p->current->channel_id),"%s",attrs[i+1]);else if(!strcmp(attrs[i],"start"))parse_ts(attrs[i+1],&p->current->start_ms);else if(!strcmp(attrs[i],"stop"))parse_ts(attrs[i+1],&p->current->end_ms);}}else if(p->current&&!strcmp(name,"title"))snprintf(p->active,sizeof(p->active),"%s","title");else if(p->current&&!strcmp(name,"desc"))snprintf(p->active,sizeof(p->active),"%s","description");}
static void XMLCALL chars(void *ud,const char *s,int n){parser_ctx *p=ud;if(!p->current||!p->active[0])return;size_t take=(size_t)n;if(p->text_len+take>=sizeof(p->text))take=sizeof(p->text)-p->text_len-1;memcpy(p->text+p->text_len,s,take);p->text_len+=take;p->text[p->text_len]=0;}
static void XMLCALL end(void *ud,const char *name){parser_ctx *p=ud;if(!p->current)return;if(!strcmp(name,"title")){snprintf(p->current->title,sizeof(p->current->title),"%s",p->text);p->active[0]=0;}else if(!strcmp(name,"desc")){snprintf(p->current->description,sizeof(p->current->description),"%s",p->text);p->active[0]=0;}else if(!strcmp(name,"programme")){if(p->current->channel_id[0]&&p->current->end_ms>=p->current->start_ms)p->doc->event_count++;p->current=NULL;p->active[0]=0;}}
int sheen_epg_parse_xmltv(const char *path,sheen_epg_document *d){if(!path||!d)return EINVAL;memset(d,0,sizeof(*d));FILE *f=fopen(path,"rb");if(!f)return errno;XML_Parser x=XML_ParserCreate(NULL);if(!x){fclose(f);return ENOMEM;}parser_ctx ctx={.doc=d};XML_SetUserData(x,&ctx);XML_SetElementHandler(x,start,end);XML_SetCharacterDataHandler(x,chars);char buf[8192];int ok=1;size_t n;while((n=fread(buf,1,sizeof(buf),f))>0){if(XML_Parse(x,buf,(int)n,n==0)==XML_STATUS_ERROR){ok=0;break;}}if(ferror(f))ok=0;fclose(f);if(XML_Parse(x,NULL,0,1)==XML_STATUS_ERROR)ok=0;XML_ParserFree(x);return ok?0:EPROTO;}
int sheen_epg_find(const sheen_epg_document *d,const char *channel_id,int64_t pos,size_t *first,size_t *count){if(!d||!channel_id||!first||!count)return EINVAL;*first=0;*count=0;for(size_t i=0;i<d->event_count;i++)if(!strcmp(d->events[i].channel_id,channel_id)&&pos>=d->events[i].start_ms&&pos<d->events[i].end_ms){if(!*count)*first=i;(*count)++;}return 0;}
