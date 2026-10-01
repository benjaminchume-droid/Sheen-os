#define _GNU_SOURCE
#include <errno.h>
#include <linux/input.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "sheen/android-input.h"

static int map_key(unsigned code){
    switch(code){
        case KEY_HOME:return 3;
        case KEY_BACK:return 4;
        case KEY_0:return 7;
        case KEY_1:return 8;
        case KEY_2:return 9;
        case KEY_3:return 10;
        case KEY_4:return 11;
        case KEY_5:return 12;
        case KEY_6:return 13;
        case KEY_7:return 14;
        case KEY_8:return 15;
        case KEY_9:return 16;
        case KEY_UP:return 19;
        case KEY_DOWN:return 20;
        case KEY_LEFT:return 21;
        case KEY_RIGHT:return 22;
        case KEY_VOLUMEUP:return 24;
        case KEY_VOLUMEDOWN:return 25;
        case KEY_POWER:return 26;
        case KEY_ENTER:return 66;
        case KEY_BACKSPACE:return 67;
        case KEY_MENU:return 82;
        case KEY_PLAYPAUSE:return 85;
        case KEY_STOPCD:return 86;
        case KEY_NEXTSONG:return 87;
        case KEY_PREVIOUSSONG:return 88;
        case KEY_MUTE:return 164;
        case KEY_CHANNELUP:return 166;
        case KEY_CHANNELDOWN:return 167;
        case KEY_RECORD:return 130;
        default:return -1;
    }
}
static int action_for_value(int value){
    return value==0?1:value==1?0:2;
}
int sheen_android_translate_evdev(const struct input_event *in,sheen_android_input_event *out){
    if(!in||!out)return EINVAL;
    memset(out,0,sizeof(*out));
    out->time_ns=(int64_t)in->input_event_sec*1000000000LL+(int64_t)in->input_event_usec*1000LL;
    if(in->type==EV_KEY){
        int key=map_key(in->code);
        if(key<0)return ENOTSUP;
        out->type=SHEEN_ANDROID_INPUT_KEY;
        out->android_code=key;
        out->action=action_for_value(in->value);
        out->value=in->value;
        return 0;
    }
    if(in->type==EV_ABS){
        out->type=SHEEN_ANDROID_INPUT_TOUCH;
        if(in->code==ABS_X||in->code==ABS_MT_POSITION_X){
            out->x=in->value;
            return 0;
        }
        if(in->code==ABS_Y||in->code==ABS_MT_POSITION_Y){
            out->y=in->value;
            return 0;
        }
    }
    if(in->type==EV_SYN)return 0;
    return ENOTSUP;
}
int sheen_android_input_poll(int fd,sheen_android_input_event *out){
    if(fd<0||!out)return EINVAL;
    struct input_event in;
    ssize_t n=read(fd,&in,sizeof(in));
    if(n<0)return errno;
    if(n!=(ssize_t)sizeof(in))return EPROTO;
    return sheen_android_translate_evdev(&in,out);
}
const char *sheen_android_key_name(int code){
    switch(code){
        case 3:return "HOME";
        case 4:return "BACK";
        case 19:return "DPAD_UP";
        case 20:return "DPAD_DOWN";
        case 21:return "DPAD_LEFT";
        case 22:return "DPAD_RIGHT";
        case 24:return "VOLUME_UP";
        case 25:return "VOLUME_DOWN";
        case 26:return "POWER";
        case 66:return "ENTER";
        case 82:return "MENU";
        case 85:return "MEDIA_PLAY_PAUSE";
        case 166:return "CHANNEL_UP";
        case 167:return "CHANNEL_DOWN";
        default:return "UNKNOWN";
    }
}
