#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/demux.h"
#include "sheen/container.h"

static uint16_t u16(const unsigned char *p){return (uint16_t)((p[0]<<8)|p[1]);}
static int find_pmt_pid(const unsigned char *s,size_t n,uint16_t *program,uint16_t *pmt){
    if(n<12||s[0]!=0x00)return -1;size_t len=((s[1]&0x0f)<<8)|s[2];if(3+len>n)return -1;size_t p=8;size_t end=3+len-4;
    while(p+4<=end){uint16_t pn=u16(s+p);uint16_t pid=(uint16_t)(((s[p+2]&0x1f)<<8)|s[p+3]);p+=4;if(pn){*program=pn;*pmt=pid;return 0;}}return -1;
}
static int parse_pmt(const unsigned char *s,size_t n,sheen_demux_program *prog){
    if(n<12||s[0]!=0x02)return -1;size_t len=((s[1]&0x0f)<<8)|s[2];if(3+len>n)return -1;prog->program_number=u16(s+3);prog->pcr_pid=(uint16_t)(((s[8]&0x1f)<<8)|s[9]);size_t info=12+(((s[10]&0x0f)<<8)|s[11]);size_t end=3+len-4;if(info>end)return -1;size_t p=info;uint8_t idx=0;
    while(p+5<=end&&prog->stream_count<SHEEN_DEMUX_MAX_STREAMS){uint8_t st=s[p];uint16_t pid=(uint16_t)(((s[p+1]&0x1f)<<8)|s[p+2]);size_t es=6+(((s[p+3]&0x0f)<<8)|s[p+4]);if(p+es>end)break;prog->streams[prog->stream_count++]=(sheen_demux_stream){pid,st,idx++};p+=es;}return 0;
}
int sheen_demux_mpegts_file(const char *path,sheen_demux_result *r){
    if(!path||!r)return EINVAL;memset(r,0,sizeof(*r));snprintf(r->source,sizeof(r->source),"%s",path);snprintf(r->container,sizeof(r->container),"%s","mpeg-ts");
    FILE *f=fopen(path,"rb");if(!f)return errno;unsigned char pkt[188],pat[1024]={0},pmt[4096]={0};size_t pat_len=0,pmt_len=0;uint16_t program=0,pmt_pid=0;int have_pat=0,have_pmt=0;
    while(fread(pkt,1,sizeof(pkt),f)==sizeof(pkt)){if(pkt[0]!=0x47)continue;uint16_t pid=(uint16_t)(((pkt[1]&0x1f)<<8)|pkt[2]);int start=(pkt[1]&0x40)!=0;int afc=(pkt[3]>>4)&3;size_t off=4;if(afc==0||afc==2)continue;if(afc==3){if(off>=188)continue;size_t al=pkt[off]+1;if(off+al>188)continue;off+=al;}if(off>=188)continue;size_t pay=188-off;
        if(pid==0){if(start){size_t ptr=pkt[off];if(ptr+1>=pay)continue;off+=1+ptr;pay=188-off;pat_len=0;}if(pat_len+pay>sizeof(pat))pay=sizeof(pat)-pat_len;memcpy(pat+pat_len,pkt+off,pay);pat_len+=pay;if(find_pmt_pid(pat,pat_len,&program,&pmt_pid)==0)have_pat=1;}
        else if(have_pat&&pid==pmt_pid){if(start){size_t ptr=pkt[off];if(ptr+1>=pay)continue;off+=1+ptr;pay=188-off;pmt_len=0;}if(pmt_len+pay>sizeof(pmt))pay=sizeof(pmt)-pmt_len;memcpy(pmt+pmt_len,pkt+off,pay);pmt_len+=pay;if(r->program_count==0&&parse_pmt(pmt,pmt_len,&r->programs[0])==0){r->programs[0].pmt_pid=pmt_pid;r->program_count=1;have_pmt=1;break;}}
        if(have_pmt)break;
    }
    fclose(f);return 0;
}
