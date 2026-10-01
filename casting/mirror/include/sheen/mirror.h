#ifndef SHEEN_MIRROR_H
#define SHEEN_MIRROR_H
#include <stddef.h>
#include <stdint.h>
typedef struct sheen_mirror_receiver sheen_mirror_receiver;
typedef struct {
    uint32_t ssrc;
    uint16_t sequence;
    uint32_t timestamp;
    uint8_t nal_type;
    size_t bytes;
    int marker;
} sheen_mirror_packet_info;
sheen_mirror_receiver *sheen_mirror_open(uint16_t port);
int sheen_mirror_receive(sheen_mirror_receiver *receiver,uint8_t *frame,size_t capacity,sheen_mirror_packet_info *info);
int sheen_mirror_port(const sheen_mirror_receiver *receiver);
void sheen_mirror_close(sheen_mirror_receiver *receiver);
#endif
