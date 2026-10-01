#include <stdio.h>
#include <string.h>
#include "sheen/decode.h"
struct ctx{int opens;int sends;int flushes;};
static int openb(void **out,const char *id,const void *c,size_t n){(void)c;(void)n;struct ctx *x=calloc(1,sizeof(*x));if(!x)return 12;x->opens++;*out=x;printf("backend open %s\\n",id);return 0;}
static int sendb(void *ctx,const sheen_packet *p){struct ctx *x=ctx;x->sends++;return p->size?0:0;}
static int recvb(void *ctx,sheen_frame *f){(void)ctx;(void)f;return EAGAIN;}
static int flushb(void *ctx){((struct ctx*)ctx)->flushes++;return 0;}
static void closeb(void *ctx){free(ctx);}
static const sheen_decoder_backend backend={openb,sendb,recvb,flushb,closeb};
int main(void){sheen_decoder_pipeline *p=sheen_decode_create(&backend);if(!p)return 1;if(sheen_decode_open(p,"test.codec",NULL,0))return 1;unsigned char bytes[]={1,2,3,4};if(sheen_decode_send_packet(p,bytes,sizeof(bytes),10,9))return 1;if(sheen_decode_flush(p))return 1;if(sheen_decode_state(p)!=SHEEN_DECODER_DRAINING)return 1;sheen_decode_destroy(p);puts("decoder pipeline ok");return 0;}
