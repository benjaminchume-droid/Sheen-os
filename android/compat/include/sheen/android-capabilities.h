#ifndef SHEEN_ANDROID_CAPABILITIES_H
#define SHEEN_ANDROID_CAPABILITIES_H
#include <stddef.h>
#include <stdint.h>
typedef enum {
    SHEEN_ANDROID_FEATURE_RUNTIME = 0,
    SHEEN_ANDROID_FEATURE_BINDER,
    SHEEN_ANDROID_FEATURE_GRAPHICS_DMABUF,
    SHEEN_ANDROID_FEATURE_INPUT_EVDEV,
    SHEEN_ANDROID_FEATURE_AUDIO_PCM,
    SHEEN_ANDROID_FEATURE_STORAGE_SANDBOX,
    SHEEN_ANDROID_FEATURE_PERMISSIONS,
    SHEEN_ANDROID_FEATURE_TV_PROFILE,
    SHEEN_ANDROID_FEATURE_APK_PACKAGE,
    SHEEN_ANDROID_FEATURE_COUNT
} sheen_android_feature;
typedef struct {
    uint64_t supported;
    uint64_t available_now;
} sheen_android_capabilities;
int sheen_android_capabilities_probe(sheen_android_capabilities *capabilities);
int sheen_android_capability_supported(const sheen_android_capabilities *capabilities,
                                       sheen_android_feature feature);
int sheen_android_capability_available(const sheen_android_capabilities *capabilities,
                                       sheen_android_feature feature);
const char *sheen_android_feature_name(sheen_android_feature feature);
#endif
