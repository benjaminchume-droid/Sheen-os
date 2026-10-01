#ifndef SHEEN_ANDROID_INPUT_H
#define SHEEN_ANDROID_INPUT_H
#include <stdint.h>
#include <linux/input.h>
typedef enum {
    SHEEN_ANDROID_INPUT_KEY,
    SHEEN_ANDROID_INPUT_TOUCH
} sheen_android_input_type;
typedef struct {
    sheen_android_input_type type;
    int android_code;
    int action;
    int32_t x;
    int32_t y;
    int32_t value;
    int64_t time_ns;
} sheen_android_input_event;
int sheen_android_translate_evdev(const struct input_event *input,
                                  sheen_android_input_event *output);
int sheen_android_input_poll(int fd,sheen_android_input_event *output);
const char *sheen_android_key_name(int android_code);
#endif
