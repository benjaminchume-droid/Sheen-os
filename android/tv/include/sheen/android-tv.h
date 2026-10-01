#ifndef SHEEN_ANDROID_TV_H
#define SHEEN_ANDROID_TV_H
#include <stddef.h>
#include <stdint.h>
typedef enum {
    SHEEN_ANDROID_TV_FOCUS_UP,
    SHEEN_ANDROID_TV_FOCUS_DOWN,
    SHEEN_ANDROID_TV_FOCUS_LEFT,
    SHEEN_ANDROID_TV_FOCUS_RIGHT,
    SHEEN_ANDROID_TV_FOCUS_ENTER,
    SHEEN_ANDROID_TV_BACK,
    SHEEN_ANDROID_TV_HOME,
    SHEEN_ANDROID_TV_MENU,
    SHEEN_ANDROID_TV_PLAY_PAUSE,
    SHEEN_ANDROID_TV_CHANNEL_UP,
    SHEEN_ANDROID_TV_CHANNEL_DOWN,
    SHEEN_ANDROID_TV_VOLUME_UP,
    SHEEN_ANDROID_TV_VOLUME_DOWN,
    SHEEN_ANDROID_TV_MUTE,
    SHEEN_ANDROID_TV_POWER
} sheen_android_tv_action;
typedef struct {
    uint32_t viewport_width;
    uint32_t viewport_height;
    uint32_t min_focus_target_px;
    uint32_t repeat_delay_ms;
    uint32_t repeat_interval_ms;
    int touch_optional;
} sheen_android_tv_profile;
int sheen_android_tv_profile_init(sheen_android_tv_profile *profile,
                                  uint32_t width,uint32_t height);
int sheen_android_tv_action_from_key(int android_keycode,
                                     sheen_android_tv_action *action);
const char *sheen_android_tv_action_name(sheen_android_tv_action action);
#endif
