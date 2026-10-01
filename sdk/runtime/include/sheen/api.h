#ifndef SHEEN_API_H
#define SHEEN_API_H
#include <stddef.h>
#include <stdint.h>
typedef struct sheen_api_client sheen_api_client;
typedef struct {
    int fd;
    char bus_path[108];
    uint32_t request_timeout_ms;
} sheen_api_transport;
typedef struct {
    char status[32];
    char request_id[128];
    char payload[65536];
} sheen_api_response;
sheen_api_client *sheen_api_connect(const char *bus_path,uint32_t timeout_ms);
int sheen_api_request(sheen_api_client *client,const char *service,const char *operation,
                      const char *payload,sheen_api_response *response);
int sheen_api_subscribe(sheen_api_client *client,const char *service,const char *event);
int sheen_api_next_event(sheen_api_client *client,char *event_json,size_t capacity);
void sheen_api_close(sheen_api_client *client);
const char *sheen_api_transport_name(void);
#endif
