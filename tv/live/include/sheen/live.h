#ifndef SHEEN_LIVE_H
#define SHEEN_LIVE_H
#include <stddef.h>
#include <stdint.h>
#include "sheen/stream.h"
typedef enum { SHEEN_LIVE_OPENING, SHEEN_LIVE_READY, SHEEN_LIVE_PLAYING, SHEEN_LIVE_PAUSED, SHEEN_LIVE_STOPPED, SHEEN_LIVE_FAILED } sheen_live_state;
typedef struct sheen_live_session sheen_live_session;
typedef struct { char session_id[65]; char source[4096]; sheen_live_state state; uint64_t bytes_read; } sheen_live_info;
sheen_live_session *sheen_live_open(const char *source);
int sheen_live_play(sheen_live_session *session);
int sheen_live_pause(sheen_live_session *session);
ssize_t sheen_live_read(sheen_live_session *session,void *buffer,size_t capacity);
int sheen_live_info_get(const sheen_live_session *session,sheen_live_info *info);
int sheen_live_stop(sheen_live_session *session);
void sheen_live_close(sheen_live_session *session);
const char *sheen_live_state_name(sheen_live_state state);
#endif
