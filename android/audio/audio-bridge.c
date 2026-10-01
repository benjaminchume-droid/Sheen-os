#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-audio.h"

struct sheen_android_audio {
    sheen_audio_pipeline *pipeline;
    sheen_android_audio_format android_format;
};

static sheen_audio_sample_format map_encoding(sheen_android_pcm_encoding e) {
    switch(e) {
        case SHEEN_ANDROID_PCM_16: return SHEEN_AUDIO_S16LE;
        case SHEEN_ANDROID_PCM_FLOAT: return SHEEN_AUDIO_F32LE;
    }
    return 0;
}

sheen_android_audio *sheen_android_audio_open(const sheen_audio_sink *sink,
                                              const sheen_android_audio_format *f) {
    if(!sink||!f||!f->sample_rate||!f->channel_count)return NULL;
    sheen_audio_sample_format fmt=map_encoding(f->encoding);
    if(!fmt)return NULL;

    sheen_android_audio *a=calloc(1,sizeof(*a));
    if(!a)return NULL;
    a->pipeline=sheen_audio_create(sink);
    if(!a->pipeline){free(a);return NULL;}
    sheen_audio_format native={
        .sample_rate=f->sample_rate,
        .channels=f->channel_count,
        .format=fmt
    };
    if(sheen_audio_open(a->pipeline,&native)) {
        sheen_audio_destroy(a->pipeline);
        free(a);
        return NULL;
    }
    a->android_format=*f;
    return a;
}

int sheen_android_audio_write(sheen_android_audio *a,const void *data,
                              size_t bytes,uint64_t frames,int64_t pts) {
    if(!a||!a->pipeline)return EINVAL;
    return sheen_audio_write(a->pipeline,data,bytes,frames,pts);
}

int sheen_android_audio_flush(sheen_android_audio *a) {
    if(!a||!a->pipeline)return EINVAL;
    return sheen_audio_flush(a->pipeline);
}

void sheen_android_audio_close(sheen_android_audio *a) {
    if(!a)return;
    sheen_audio_destroy(a->pipeline);
    free(a);
}
