#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/decode.h"
struct item { uint8_t *data; size_t size; int64_t pts; int64_t dts; };
struct sheen_decoder_pipeline { const sheen_decoder_backend *backend; void *ctx; sheen_decoder_state state; struct item q[SHEEN_DECODE_QUEUE_CAPACITY]; size_t head; size_t count; char codec_id[64]; };
sheen_decoder_pipeline *sheen_decode_create(const sheen_decoder_backend *backend){if(!backend||!backend->open||!backend->send||!backend->receive||!backend->flush||!backend->close)return NULL;sheen_decoder_pipeline *p=calloc(1,sizeof(*p));if(p){p->backend=backend;p->state=SHEEN_DECODER_CLOSED;}return p;}
void sheen_decode_destroy(sheen_decoder_pipeline *p){if(!p)return;if(p->ctx&&p->backend->close)p->backend->close(p->ctx);for(size_t i=0;i<SHEEN_DECODE_QUEUE_CAPACITY;i++)free(p->q[i].data);free(p);}
int sheen_decode_open(sheen_decoder_pipeline *p,const char *codec_id,const void *config,size_t config_size){if(!p||!codec_id||p->state!=SHEEN_DECODER_CLOSED)return EINVAL;int rc=p->backend->open(&p->ctx,codec_id,config,config_size);if(rc){p->state=SHEEN_DECODER_ERROR;return rc;}snprintf(p->codec_id,sizeof(p->codec_id),"%s",codec_id);p->state=SHEEN_DECODER_OPEN;return 0;}
int sheen_decode_send_packet(sheen_decoder_pipeline *p,const uint8_t *data,size_t size,int64_t pts,int64_t dts){if(!p||p->state!=SHEEN_DECODER_OPEN||(!data&&size)||size>SHEEN_DECODE_PACKET_MAX)return EINVAL;if(p->count>=SHEEN_DECODE_QUEUE_CAPACITY)return EAGAIN;struct item *i=&p->q[(p->head+p->count)%SHEEN_DECODE_QUEUE_CAPACITY];i->data=malloc(size?size:1);if(!i->data)return ENOMEM;if(size)memcpy(i->data,data,size);i->size=size;i->pts=pts;i->dts=dts;p->count++;return p->backend->send(p->ctx,&(sheen_packet){i->data,i->size,pts,dts});}
int sheen_decode_receive_frame(sheen_decoder_pipeline *p,sheen_frame *f){if(!p||!f)return EINVAL;if(p->state!=SHEEN_DECODER_OPEN&&p->state!=SHEEN_DECODER_DRAINING&&p->state!=SHEEN_DECODER_EOF)return EPIPE;int rc=p->backend->receive(p->ctx,f);if(rc==0)return 0;if(rc==EPIPE&&p->state==SHEEN_DECODER_DRAINING){p->state=SHEEN_DECODER_EOF;}return rc;}
int sheen_decode_flush(sheen_decoder_pipeline *p){if(!p||!p->ctx||p->state!=SHEEN_DECODER_OPEN)return EINVAL;int rc=p->backend->flush(p->ctx);if(rc==0)p->state=SHEEN_DECODER_DRAINING;else p->state=SHEEN_DECODER_ERROR;return rc;}
sheen_decoder_state sheen_decode_state(const sheen_decoder_pipeline *p){return p?p->state:SHEEN_DECODER_ERROR;}
