#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/audio.h"
struct sheen_audio_pipeline { const sheen_audio_sink *sink; void *ctx; sheen_audio_state state; sheen_audio_format format; };
sheen_audio_pipeline *sheen_audio_create(const sheen_audio_sink *sink){if(!sink||!sink->open||!sink->write||!sink->flush||!sink->close)return NULL;sheen_audio_pipeline *p=calloc(1,sizeof(*p));if(p){p->sink=sink;p->state=SHEEN_AUDIO_CLOSED;}return p;}
void sheen_audio_destroy(sheen_audio_pipeline *p){if(!p)return;if(p->ctx)p->sink->close(p->ctx);free(p);}
int sheen_audio_open(sheen_audio_pipeline *p,const sheen_audio_format *f){if(!p||!f||p->state!=SHEEN_AUDIO_CLOSED||!f->sample_rate||!f->channels)return EINVAL;int rc=p->sink->open(&p->ctx,f);if(rc){p->state=SHEEN_AUDIO_ERROR;return rc;}p->format=*f;p->state=SHEEN_AUDIO_OPEN;return 0;}
int sheen_audio_write(sheen_audio_pipeline *p,const uint8_t *data,size_t bytes,uint64_t frames,int64_t pts){if(!p||p->state!=SHEEN_AUDIO_OPEN||(!data&&bytes))return EINVAL;if(frames==0)return EINVAL;sheen_audio_buffer b={data,bytes,frames,pts,p->format};int rc=p->sink->write(p->ctx,&b);if(rc&&rc!=EAGAIN)p->state=SHEEN_AUDIO_ERROR;return rc;}
int sheen_audio_flush(sheen_audio_pipeline *p){if(!p||!p->ctx||p->state!=SHEEN_AUDIO_OPEN)return EINVAL;int rc=p->sink->flush(p->ctx);if(!rc)p->state=SHEEN_AUDIO_DRAINING;else p->state=SHEEN_AUDIO_ERROR;return rc;}
sheen_audio_state sheen_audio_state_get(const sheen_audio_pipeline *p){return p?p->state:SHEEN_AUDIO_ERROR;}
