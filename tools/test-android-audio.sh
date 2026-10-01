#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-audio.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-audio.h"
struct ctx { int writes; int flushed; };
static int openb(void **out,const sheen_audio_format *f){ if(f->sample_rate!=48000||f->channels!=2)return EINVAL; struct ctx *c=calloc(1,sizeof(*c));if(!c)return ENOMEM;*out=c;return 0; }
static int writeb(void *ctx,const sheen_audio_buffer *b){ struct ctx *c=ctx;c->writes++;return b->frames?0:EINVAL; }
static int flushb(void *ctx){((struct ctx*)ctx)->flushed=1;return 0;}
static void closeb(void *ctx){free(ctx);}
static const sheen_audio_sink sink={openb,writeb,flushb,closeb};
int main(void){
 sheen_android_audio_format f={48000,2,SHEEN_ANDROID_PCM_16};
 sheen_android_audio *a=sheen_android_audio_open(&sink,&f);if(!a)return 1;short pcm[4]={0};if(sheen_android_audio_write(a,pcm,sizeof(pcm),2,123))return 2;if(sheen_android_audio_flush(a))return 3;sheen_android_audio_close(a);
 f.encoding=SHEEN_ANDROID_PCM_FLOAT;a=sheen_android_audio_open(&sink,&f);if(!a)return 4;float fp[4]={0};if(sheen_android_audio_write(a,fp,sizeof(fp),2,456))return 5;sheen_android_audio_close(a);puts("android audio ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/audio/include" -I"$ROOT/android/audio/include" "$ROOT/media/audio/audio-pipeline.c" "$ROOT/android/audio/audio-bridge.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "android audio ok"
echo "Android audio bridge tests passed"
