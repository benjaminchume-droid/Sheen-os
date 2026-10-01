#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/webrtc.h"

static char *trim(char *s) {
    while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n') s++;
    char *e=s+strlen(s);
    while(e>s && (e[-1]==' '||e[-1]=='\t'||e[-1]=='\r'||e[-1]=='\n')) *--e=0;
    return s;
}
static int add_candidate(sheen_webrtc_offer *o,const char *v) {
    if(o->candidate_count>=SHEEN_WEBRTC_MAX_CANDIDATES) return 0;
    char copy[1024];
    snprintf(copy,sizeof(copy),"%s",v);
    char *save=NULL;
    char *tok=strtok_r(copy," ",&save);
    char *fields[16]; int n=0;
    while(tok && n<16){fields[n++]=tok;tok=strtok_r(NULL," ",&save);}
    if(n<8)return 0;
    sheen_webrtc_candidate *c=&o->candidates[o->candidate_count];
    memset(c,0,sizeof(*c));
    snprintf(c->foundation,sizeof(c->foundation),"%s",fields[0]);
    c->component=(uint32_t)strtoul(fields[1],NULL,10);
    snprintf(c->transport,sizeof(c->transport),"%s",fields[2]);
    c->priority=(uint32_t)strtoul(fields[3],NULL,10);
    snprintf(c->address,sizeof(c->address),"%s",fields[4]);
    c->port=(uint16_t)strtoul(fields[5],NULL,10);
    snprintf(c->type,sizeof(c->type),"%s",fields[7]);
    o->candidate_count++;
    return 0;
}
int sheen_webrtc_parse_sdp(const char *sdp,sheen_webrtc_offer *o){
    if(!sdp||!o)return EINVAL;
    memset(o,0,sizeof(*o));
    char *copy=strdup(sdp);if(!copy)return ENOMEM;
    char *save=NULL;char *line=strtok_r(copy,"\n",&save);
    while(line){
        line=trim(line);
        if(!strncmp(line,"a=ice-ufrag:",12))snprintf(o->ice_ufrag,sizeof(o->ice_ufrag),"%s",line+12);
        else if(!strncmp(line,"a=ice-pwd:",10))snprintf(o->ice_pwd,sizeof(o->ice_pwd),"%s",line+10);
        else if(!strncmp(line,"m=video ",8))o->has_video=1;
        else if(!strncmp(line,"m=audio ",8))o->has_audio=1;
        else if(!strncmp(line,"a=rtpmap:",9)){
            if(o->codec_count<SHEEN_WEBRTC_MAX_CODECS){
                char *p=line+9;int pt=(int)strtol(p,&p,10);if(*p==' ')p++;
                sheen_webrtc_codec *c=&o->codecs[o->codec_count];
                char rate[64]={0},name[64]={0};unsigned channels=0;
                int n=sscanf(p,"%63[^/]/%63u/%u",name,&c->clock_rate,&channels);
                if(n>=2){c->payload_type=pt;snprintf(c->encoding,sizeof(c->encoding),"%s",name);c->channels=(uint8_t)(n==3?channels:1);o->codec_count++;}
            }
        } else if(!strncmp(line,"a=candidate:",12))add_candidate(o,line+12);
        line=strtok_r(NULL,"\n",&save);
    }
    free(copy);
    return 0;
}
