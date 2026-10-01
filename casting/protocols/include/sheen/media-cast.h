#ifndef SHEEN_MEDIA_CAST_H
#define SHEEN_MEDIA_CAST_H
#include <stddef.h>
typedef enum { SHEEN_CAST_SESSION_CREATED, SHEEN_CAST_SESSION_LOADING, SHEEN_CAST_SESSION_PLAYING, SHEEN_CAST_SESSION_STOPPED, SHEEN_CAST_SESSION_FAILED } sheen_cast_media_state;
typedef struct { char device_id[256]; char location[4096]; char control_url[4096]; } sheen_cast_renderer;
typedef struct { sheen_cast_renderer renderer; char media_uri[4096]; sheen_cast_media_state state; } sheen_cast_media_session;
int sheen_cast_renderer_describe(const char *location,sheen_cast_renderer *renderer);
int sheen_cast_media_set_uri(sheen_cast_media_session *session,const char *media_uri);
int sheen_cast_media_play(sheen_cast_media_session *session);
int sheen_cast_media_stop(sheen_cast_media_session *session);
const char *sheen_cast_media_state_name(sheen_cast_media_state state);
#endif
