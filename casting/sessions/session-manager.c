#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sheen/cast-session.h"

#define MAX_SESSIONS 128

typedef struct {
    sheen_cast_session_info info;
    const sheen_cast_session_backend *backend;
    void *ctx;
} session_slot;

struct sheen_cast_session_manager {
    session_slot slots[MAX_SESSIONS];
};

static uint64_t hash64(const void *data,size_t n,uint64_t h){
    const unsigned char *p=data;
    for(size_t i=0;i<n;i++){ h^=p[i]; h*=1099511628211ULL; }
    return h;
}
static void make_id(const char *device,const char *mode,const char *options,char out[65]){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    uint64_t h=1469598103934665603ULL;
    h=hash64(device,strlen(device),h);
    h=hash64(mode,strlen(mode),h);
    h=hash64(options?options:"",options?strlen(options):0,h);
    h=hash64(&ts,sizeof(ts),h);
    snprintf(out,65,"%016llx",(unsigned long long)h);
}
sheen_cast_session_manager *sheen_cast_session_manager_create(void){
    return calloc(1,sizeof(sheen_cast_session_manager));
}
void sheen_cast_session_manager_destroy(sheen_cast_session_manager *m){
    if(!m) return;
    for(size_t i=0;i<MAX_SESSIONS;i++){
        if(m->slots[i].backend && m->slots[i].ctx)
            m->slots[i].backend->stop(m->slots[i].ctx);
        if(m->slots[i].backend && m->slots[i].ctx)
            m->slots[i].backend->close(m->slots[i].ctx);
    }
    free(m);
}
int sheen_cast_session_create(sheen_cast_session_manager *m,const char *device,const char *mode,
                              const char *options,const sheen_cast_session_backend *backend,
                              char out_id[65]){
    if(!m||!device||!mode||!out_id) return EINVAL;
    size_t slot=MAX_SESSIONS;
    for(size_t i=0;i<MAX_SESSIONS;i++) if(!m->slots[i].info.session_id[0]){slot=i;break;}
    if(slot==MAX_SESSIONS) return ENOSPC;
    session_slot *s=&m->slots[slot];
    memset(s,0,sizeof(*s));
    snprintf(s->info.device_id,sizeof(s->info.device_id),"%s",device);
    snprintf(s->info.mode,sizeof(s->info.mode),"%s",mode);
    make_id(device,mode,options,s->info.session_id);
    snprintf(out_id,65,"%s",s->info.session_id);
    s->info.state=SHEEN_CAST_SESSION_CREATED;
    if(!backend||!backend->open||!backend->stop||!backend->close){
        s->info.state=SHEEN_CAST_SESSION_UNSUPPORTED;
        return ENOTSUP;
    }
    s->backend=backend;
    s->info.state=SHEEN_CAST_SESSION_CONNECTING;
    int rc=backend->open(&s->ctx,&s->info,options?options:"");
    if(rc){
        s->info.state=(rc==ENOTSUP)?SHEEN_CAST_SESSION_UNSUPPORTED:SHEEN_CAST_SESSION_FAILED;
        s->ctx=NULL;
        return rc;
    }
    s->info.state=SHEEN_CAST_SESSION_ACTIVE;
    return 0;
}
int sheen_cast_session_stop(sheen_cast_session_manager *m,const char *id){
    if(!m||!id)return EINVAL;
    for(size_t i=0;i<MAX_SESSIONS;i++) if(!strcmp(m->slots[i].info.session_id,id)){
        session_slot *s=&m->slots[i];
        if(s->backend&&s->ctx){
            s->info.state=SHEEN_CAST_SESSION_STOPPING;
            int rc=s->backend->stop(s->ctx);
            if(rc){s->info.state=SHEEN_CAST_SESSION_FAILED;return rc;}
            s->backend->close(s->ctx);
            s->ctx=NULL;
        }
        s->info.state=SHEEN_CAST_SESSION_STOPPED;
        return 0;
    }
    return ENOENT;
}
int sheen_cast_session_get(sheen_cast_session_manager *m,const char *id,sheen_cast_session_info *out){
    if(!m||!id||!out)return EINVAL;
    for(size_t i=0;i<MAX_SESSIONS;i++)if(!strcmp(m->slots[i].info.session_id,id)){*out=m->slots[i].info;return 0;}
    return ENOENT;
}
int sheen_cast_session_count(sheen_cast_session_manager *m,size_t *count){
    if(!m||!count)return EINVAL;
    *count=0;
    for(size_t i=0;i<MAX_SESSIONS;i++)if(m->slots[i].info.session_id[0]&&m->slots[i].info.state!=SHEEN_CAST_SESSION_STOPPED)(*count)++;
    return 0;
}
