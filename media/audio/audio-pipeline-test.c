#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "sheen/audio.h"
struct ctx { int writes; int flushes; }; static int reject_next=1;
static int openb(void **out,const sheen_audio_format *f){struct ctx *c=calloc(1,sizeof(*c));if(!c)return ENOMEM;printf("open %uHz %uch\\n",f->sample_rate,f->channels);*out=c;return 0;}
static int writeb(void *ctx,const sheen_audio_buffer *b){struct ctx *c=ctx;c->writes++;if(reject_next){reject_next=0;return EAGAIN;}return b->bytes?0:EIO;}
static int flushb(void *ctx){((struct ctx*)ctx)->flushes++;return 0;} static void closeb(void *ctx){free(ctx);}
static const sheen_audio_sink sink={openb,writeb,flushb,closeb};
int main(void){sheen_audio_format f={48000,2,SHEEN_AUDIO_S16LE};sheen_audio_pipeline *p=sheen_audio_create(&sink);if(!p)return 1;if(sheen_audio_open(p,&f))return 1;uint8_t pcm[8]={0};if(sheen_audio_write(p,pcm,sizeof(pcm),2,100)!=EAGAIN)return 1;if(sheen_audio_write(p,pcm,sizeof(pcm),2,100)!=0)return 1;if(sheen_audio_flush(p)!=0)return 1;if(sheen_audio_state_get(p)!=SHEEN_AUDIO_DRAINING)return 1;sheen_audio_destroy(p);puts("audio pipeline ok");return 0;}
