#ifndef SHEEN_BROADCAST_H
#define SHEEN_BROADCAST_H
#include <stddef.h>
#include <stdint.h>
#include "sheen/demux.h"
#include "sheen/tuner.h"
typedef enum { SHEEN_BROADCAST_TERRESTRIAL, SHEEN_BROADCAST_CABLE, SHEEN_BROADCAST_SATELLITE, SHEEN_BROADCAST_ATSC, SHEEN_BROADCAST_ISDB, SHEEN_BROADCAST_NETWORK } sheen_broadcast_type;
typedef struct { sheen_broadcast_type type; sheen_tuner_delivery delivery; uint32_t frequency_hz; uint32_t bandwidth_hz; uint16_t transport_stream_id; uint16_t original_network_id; } sheen_broadcast_source;
typedef struct { uint16_t service_id; uint16_t program_number; uint16_t pmt_pid; uint16_t pcr_pid; size_t stream_count; sheen_demux_stream streams[SHEEN_DEMUX_MAX_STREAMS]; char service_name[256]; char provider_name[256]; } sheen_broadcast_program;
typedef struct { sheen_broadcast_source source; size_t program_count; sheen_broadcast_program programs[16]; } sheen_broadcast_multiplex;
int sheen_broadcast_from_ts(const char *path,const sheen_broadcast_source *source,sheen_broadcast_multiplex *out);
const char *sheen_broadcast_type_name(sheen_broadcast_type type);
#endif
