#ifndef SHEEN_WFD_H
#define SHEEN_WFD_H
#include <stdint.h>
typedef struct sheen_wfd_server sheen_wfd_server;
typedef struct {
    uint16_t rtsp_port;
    uint16_t rtp_port;
    uint16_t width;
    uint16_t height;
    uint32_t fps;
} sheen_wfd_config;
sheen_wfd_server *sheen_wfd_start(const sheen_wfd_config *config);
void sheen_wfd_stop(sheen_wfd_server *server);
int sheen_wfd_port(const sheen_wfd_server *server);
