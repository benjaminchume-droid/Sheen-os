#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sheen/live.h"
struct sheen_live_session { sheen_stream *stream; sheen_live_state state; char source[4096]; char id[65]; uint64_t bytes; };
static uint64_t h64(const void *data,size_t n,uint64_t h){const unsigned char*p=data;for(size_t i=0;i<n;i++){h^=p[i];h*=1099511628211ULL;}return h;}
static void make_id(const char *source,char out[65]){uint64_t h=h64(source,strlen(source),1469598103934665603ULL);struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);h=h64(&ts,sizeof(ts),h);snprintf(out,65,"%016llx",(unsigned long long)h);}
const char *sheen_live_state_name(sheen_live_state s){switch(s){case SHEEN_LIVE_OPENING:return "opening";case SHEEN_LIVE_READY:return "ready";case SHEEN_LIVE_PLAYING:return "playing";case SHEEN_LIVE_PAUSED:return "paused";case SHEEN_LIVE_STOPPED:return "stopped";case SHEEN_LIVE_FAILED:return "failed";}return "failed";}
sheen_live_session *sheen_live_open(const char *source){if(!source)return NULL;sheen_live_session *s=calloc(1,sizeof(*s));if(!s)return NULL;s->state=SHEEN_LIVE_OPENING;snprintf(s->source,sizeof(s->source),"%s",source);make_id(source,s->id);s->stream=sheen_stream_open(source);if(!s->stream){s->state=SHEEN_LIVE_FAILED;return s;}s->state=SHEEN_LIVE_READY;return s;}
int sheen_live_play(sheen_live_session *s){if(!s||!s->stream)return ENODEV;if(s->state==SHEEN_LIVE_READY||s->state==SHEEN_LIVE_PAUSED){s->state=SHEEN_LIVE_PLAYING;return 0;}return EINVAL;}
int sheen_live_pause(sheen_live_session *s){if(!s)return EINVAL;if(s->state!=SHEEN_LIVE_PLAYING)return EINVAL;s->state=SHEEN_LIVE_PAUSED;return 0;}
ssize_t sheen_live_read(sheen_live_session *s,void *b,size_t n){if(!s||!s->stream||s->state!=SHEEN_LIVE_PLAYING)return -1;ssize_t r=sheen_stream_read(s->stream,b,n);if(r<0){s->state=SHEEN_LIVE_FAILED;return -1;}if(r==0){s->state=SHEEN_LIVE_STOPPED;return 0;}s->bytes+=(uint64_t)r;return r;}
int sheen_live_info_get(const sheen_live_session *s,sheen_live_info *i){if(!s||!i)return EINVAL;memset(i,0,sizeof(*i));snprintf(i->session_id,sizeof(i->session_id),"%s",s->id);snprintf(i->source,sizeof(i->source),"%s",s->source);i->state=s->state;i->bytes_read=s->bytes;return 0;}
int sheen_live_stop(sheen_live_session *s){if(!s)return EINVAL;if(s->stream){sheen_stream_close(s->stream);s->stream=NULL;}s->state=SHEEN_LIVE_STOPPED;return 0;}
void sheen_live_close(sheen_live_session *s){if(!s)return;if(s->stream)sheen_stream_close(s->stream);free(s);}
