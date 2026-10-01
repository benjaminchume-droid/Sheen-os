#include <errno.h>
#include <stdint.h>
#include "sheen/android-tv.h"
int sheen_android_tv_profile_init(sheen_android_tv_profile *p,uint32_t w,uint32_t h){
    if(!p||!w||!h)return EINVAL;
    p->viewport_width=w;
    p->viewport_height=h;
    p->min_focus_target_px=w>=1280?64:48;
    p->repeat_delay_ms=500;
    p->repeat_interval_ms=80;
    p->touch_optional=1;
    return 0;
}
int sheen_android_tv_action_from_key(int k,sheen_android_tv_action *a){
    if(!a)return EINVAL;
    switch(k){
        case 19:*a=SHEEN_ANDROID_TV_FOCUS_UP;return 0;
        case 20:*a=SHEEN_ANDROID_TV_FOCUS_DOWN;return 0;
        case 21:*a=SHEEN_ANDROID_TV_FOCUS_LEFT;return 0;
        case 22:*a=SHEEN_ANDROID_TV_FOCUS_RIGHT;return 0;
        case 66:*a=SHEEN_ANDROID_TV_FOCUS_ENTER;return 0;
        case 4:*a=SHEEN_ANDROID_TV_BACK;return 0;
        case 3:*a=SHEEN_ANDROID_TV_HOME;return 0;
        case 82:*a=SHEEN_ANDROID_TV_MENU;return 0;
        case 85:*a=SHEEN_ANDROID_TV_PLAY_PAUSE;return 0;
        case 166:*a=SHEEN_ANDROID_TV_CHANNEL_UP;return 0;
        case 167:*a=SHEEN_ANDROID_TV_CHANNEL_DOWN;return 0;
        case 24:*a=SHEEN_ANDROID_TV_VOLUME_UP;return 0;
        case 25:*a=SHEEN_ANDROID_TV_VOLUME_DOWN;return 0;
        case 164:*a=SHEEN_ANDROID_TV_MUTE;return 0;
        case 26:*a=SHEEN_ANDROID_TV_POWER;return 0;
        default:return ENOTSUP;
    }
}
const char *sheen_android_tv_action_name(sheen_android_tv_action a){
    switch(a){
        case SHEEN_ANDROID_TV_FOCUS_UP:return "focus_up";
        case SHEEN_ANDROID_TV_FOCUS_DOWN:return "focus_down";
        case SHEEN_ANDROID_TV_FOCUS_LEFT:return "focus_left";
        case SHEEN_ANDROID_TV_FOCUS_RIGHT:return "focus_right";
        case SHEEN_ANDROID_TV_FOCUS_ENTER:return "focus_enter";
        case SHEEN_ANDROID_TV_BACK:return "back";
        case SHEEN_ANDROID_TV_HOME:return "home";
        case SHEEN_ANDROID_TV_MENU:return "menu";
        case SHEEN_ANDROID_TV_PLAY_PAUSE:return "play_pause";
        case SHEEN_ANDROID_TV_CHANNEL_UP:return "channel_up";
        case SHEEN_ANDROID_TV_CHANNEL_DOWN:return "channel_down";
        case SHEEN_ANDROID_TV_VOLUME_UP:return "volume_up";
        case SHEEN_ANDROID_TV_VOLUME_DOWN:return "volume_down";
        case SHEEN_ANDROID_TV_MUTE:return "mute";
        case SHEEN_ANDROID_TV_POWER:return "power";
    }
    return "unknown";
}
