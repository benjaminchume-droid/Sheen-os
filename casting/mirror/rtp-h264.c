#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sheen/mirror.h"

struct sheen_mirror_receiver {
    int fd;
    uint16_t port;
    uint8_t fu_buffer[4 * 1024 * 1024];
    size_t fu_len;
    uint8_t fu_type;
    uint32_t fu_timestamp;
    uint16_t fu_sequence;
    uint32_t fu_ssrc;
    int fu_active;
};

static uint16_t be16(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static uint32_t be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static int append_annexb(uint8_t *out, size_t cap, size_t *used,
                         const uint8_t *nal, size_t len) {
    if (*used + 4 + len > cap) return ENOSPC;
    out[(*used)++] = 0;
    out[(*used)++] = 0;
    out[(*used)++] = 0;
    out[(*used)++] = 1;
    memcpy(out + *used, nal, len);
    *used += len;
    return 0;
}

sheen_mirror_receiver *sheen_mirror_open(uint16_t port) {
    int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd < 0) return NULL;

    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return NULL;
    }

    sheen_mirror_receiver *r = calloc(1, sizeof(*r));
    if (!r) {
        close(fd);
        return NULL;
    }

    r->fd = fd;
    r->port = port;
    return r;
}

int sheen_mirror_receive(
    sheen_mirror_receiver *r,
    uint8_t *frame,
    size_t capacity,
    sheen_mirror_packet_info *info) {

    if (!r || !frame || !capacity || !info) return EINVAL;

    for (;;) {
        uint8_t packet[65536];
        ssize_t received = recv(r->fd, packet, sizeof(packet), 0);
        if (received < 0) {
            if (errno == EINTR) continue;
            return errno;
        }
        if (received < 12) continue;

        size_t offset = 12;
        uint8_t csrc_count = packet[0] & 0x0f;
        if (packet[0] & 0x10) {
            if (offset + 4 * csrc_count + 4 > (size_t)received) continue;
            offset += 4 * csrc_count;
            uint16_t extension_words = be16(packet + offset + 2);
            offset += 4 + (size_t)extension_words * 4;
        } else {
            if (offset + 4 * csrc_count > (size_t)received) continue;
            offset += 4 * csrc_count;
        }

        if (offset >= (size_t)received) continue;

        uint8_t payload_type = packet[1] & 0x7f;
        if (payload_type != 96 && payload_type != 97) continue;

        uint16_t sequence = be16(packet + 2);
        uint32_t timestamp = be32(packet + 4);
        uint32_t ssrc = be32(packet + 8);
        int marker = (packet[1] & 0x80) != 0;

        const uint8_t *payload = packet + offset;
        size_t payload_len = (size_t)received - offset;
        if (!payload_len) continue;

        uint8_t nal_type = payload[0] & 0x1f;

        if (nal_type >= 1 && nal_type <= 23) {
            size_t used = 0;
            int rc = append_annexb(frame, capacity, &used, payload, payload_len);
            if (rc) return rc;

            info->ssrc = ssrc;
            info->sequence = sequence;
            info->timestamp = timestamp;
            info->nal_type = nal_type;
            info->bytes = used;
            info->marker = marker;
            return (int)used;
        }

        if (nal_type != 28 || payload_len < 2) continue;

        uint8_t fu_indicator = payload[0];
        uint8_t fu_header = payload[1];
        int start = (fu_header & 0x80) != 0;
        int end = (fu_header & 0x40) != 0;
        uint8_t original_type = fu_header & 0x1f;

        if (start) {
            r->fu_len = 0;
            r->fu_type = original_type;
            r->fu_timestamp = timestamp;
            r->fu_sequence = sequence;
            r->fu_ssrc = ssrc;

            uint8_t reconstructed = (fu_indicator & 0xe0) | original_type;
            if (r->fu_len + 1 > sizeof(r->fu_buffer)) continue;
            r->fu_buffer[r->fu_len++] = reconstructed;

            if (r->fu_len + payload_len - 2 > sizeof(r->fu_buffer)) continue;
            memcpy(r->fu_buffer + r->fu_len, payload + 2, payload_len - 2);
            r->fu_len += payload_len - 2;
            r->fu_active = 1;
        } else {
            if (!r->fu_active ||
                r->fu_ssrc != ssrc ||
                r->fu_timestamp != timestamp ||
                sequence != (uint16_t)(r->fu_sequence + 1)) {
                r->fu_active = 0;
                r->fu_len = 0;
                continue;
            }

            r->fu_sequence = sequence;
            if (r->fu_len + payload_len - 2 > sizeof(r->fu_buffer)) {
                r->fu_active = 0;
                r->fu_len = 0;
                continue;
            }
            memcpy(r->fu_buffer + r->fu_len, payload + 2, payload_len - 2);
            r->fu_len += payload_len - 2;
        }

        if (end && r->fu_active) {
            size_t used = 0;
            int rc = append_annexb(frame, capacity, &used,
                                   r->fu_buffer, r->fu_len);
            if (rc) return rc;

            info->ssrc = ssrc;
            info->sequence = sequence;
            info->timestamp = timestamp;
            info->nal_type = r->fu_type;
            info->bytes = used;
            info->marker = marker;

            r->fu_active = 0;
            r->fu_len = 0;
            return (int)used;
        }
    }
}

int sheen_mirror_port(const sheen_mirror_receiver *r) {
    return r ? (int)r->port : -1;
}

void sheen_mirror_close(sheen_mirror_receiver *r) {
    if (!r) return;
    close(r->fd);
    free(r);
}
