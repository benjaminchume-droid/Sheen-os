#ifndef SHEEN_WEBRTC_H
#define SHEEN_WEBRTC_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_WEBRTC_MAX_CODECS 32
#define SHEEN_WEBRTC_MAX_CANDIDATES 64
typedef struct {
    int payload_type;
    char encoding[64];
    uint32_t clock_rate;
    uint8_t channels;
} sheen_webrtc_codec;
typedef struct {
    char foundation[64];
    uint32_t component;
    char transport[16];
    uint32_t priority;
    char address[64];
    uint16_t port;
    char type[32];
} sheen_webrtc_candidate;
typedef struct {
    char ice_ufrag[128];
    char ice_pwd[256];
    sheen_webrtc_codec codecs[SHEEN_WEBRTC_MAX_CODECS];
    size_t codec_count;
    sheen_webrtc_candidate candidates[SHEEN_WEBRTC_MAX_CANDIDATES];
    size_t candidate_count;
    int has_video;
    int has_audio;
} sheen_webrtc_offer;
int sheen_webrtc_parse_sdp(const char *sdp,sheen_webrtc_offer *offer);
typedef struct sheen_webrtc_ice sheen_webrtc_ice;
sheen_webrtc_ice *sheen_webrtc_ice_open(uint16_t port);
int sheen_webrtc_ice_fd(const sheen_webrtc_ice *ice);
int sheen_webrtc_ice_process(sheen_webrtc_ice *ice);
int sheen_webrtc_ice_port(const sheen_webrtc_ice *ice);
void sheen_webrtc_ice_close(sheen_webrtc_ice *ice);
#endif
