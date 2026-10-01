#ifndef SHEEN_DEMUX_H
#define SHEEN_DEMUX_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_DEMUX_MAX_STREAMS 64
typedef struct { uint16_t pid; uint8_t stream_type; uint8_t stream_index; } sheen_demux_stream;
typedef struct { uint16_t program_number; uint16_t pmt_pid; uint16_t pcr_pid; size_t stream_count; sheen_demux_stream streams[SHEEN_DEMUX_MAX_STREAMS]; } sheen_demux_program;
typedef struct { char source[4096]; char container[32]; size_t program_count; sheen_demux_program programs[16]; } sheen_demux_result;
int sheen_demux_mpegts_file(const char *path, sheen_demux_result *result);
#endif
