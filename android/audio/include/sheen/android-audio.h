#ifndef SHEEN_ANDROID_AUDIO_H
#define SHEEN_ANDROID_AUDIO_H
#include <stddef.h>
#include <stdint.h>
#include "sheen/audio.h"
typedef enum {
    SHEEN_ANDROID_PCM_16,
    SHEEN_ANDROID_PCM_FLOAT
} sheen_android_pcm_encoding;
typedef struct {
    uint32_t sample_rate;
    uint16_t channel_count;
    sheen_android_pcm_encoding encoding;
} sheen_android_audio_format;
typedef struct sheen_android_audio sheen_android_audio;
sheen_android_audio *sheen_android_audio_open(const sheen_audio_sink *sink,
                                              const sheen_android_audio_format *format);
int sheen_android_audio_write(sheen_android_audio *audio,
                              const void *data,size_t bytes,uint64_t frames,int64_t pts);
int sheen_android_audio_flush(sheen_android_audio *audio);
void sheen_android_audio_close(sheen_android_audio *audio);
#endif
