#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/decode.h"
struct ctx { int opens; int sends; int flushes; };
static int reject_next = 1;
static int openb(void **out,const char *id,const void *c,size_t n){(void)c;(void)n;struct ctx *x=calloc(1,sizeof(*x));if(!x)return ENOMEM;x->opens++;printf("backend open %s\\n",id);*out=x;return 0;}
static int sendb(void *ctx,const sheen_packet *p){struct ctx *x=ctx;x->sends++;if(reject_next){reject_next=0;return EAGAIN;}return p->size?0:0;}
static int recvb(void *ctx,sheen_frame *f){(void)ctx;(void)f;return EAGAIN;}
static int flushb(void *ctx){((struct ctx*)ctx)->flushes++;return 0;}
static void closeb(void *ctx){free(ctx);}
static const sheen_decoder_backend backend={openb,sendb,recvb,flushb,closeb};
int main(void){
    sheen_decoder_pipeline *p=sheen_decode_create(&backend);
    if(!p)return 1;
    if(sheen_decode_open(p,"test.codec",NULL,0))return 1;
    unsigned char bytes[]={1,2,3,4};
    if(sheen_decode_send_packet(p,bytes,sizeof(bytes),10,9)!=EAGAIN)return 1;
    if(sheen_decode_state(p)!=SHEEN_DECODER_OPEN)return 1;
    if(sheen_decode_send_packet(p,bytes,sizeof(bytes),10,9)!=0)return 1;
    sheen_frame frame={0};
    if(sheen_decode_receive_frame(p,&frame)!=EAGAIN)return 1;
    if(sheen_decode_flush(p)!=0)return 1;
    if(sheen_decode_state(p)!=SHEEN_DECODER_DRAINING)return 1;
    if(sheen_decode_receive_frame(p,&frame)!=EAGAIN)return 1;
    sheen_decode_destroy(p);
    puts("decoder pipeline ok");
    return 0;
}
