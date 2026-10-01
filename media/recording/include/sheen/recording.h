#ifndef SHEEN_RECORDING_H
#define SHEEN_RECORDING_H
#include <stddef.h>
#include <stdint.h>
typedef enum { SHEEN_RECORDING_IDLE, SHEEN_RECORDING_ACTIVE, SHEEN_RECORDING_FINALIZING, SHEEN_RECORDING_COMPLETE, SHEEN_RECORDING_ABORTED, SHEEN_RECORDING_ERROR } sheen_recording_state;
typedef struct sheen_recorder sheen_recorder;
typedef struct { char id[65]; char source[4096]; char destination[4096]; char container[32]; uint64_t bytes_written; sheen_recording_state state; } sheen_recording_info;
sheen_recorder *sheen_recording_open(const char *destination,const char *source,const char *container);
int sheen_recording_write(sheen_recorder *rec,const void *data,size_t bytes);
int sheen_recording_finalize(sheen_recorder *rec);
int sheen_recording_abort(sheen_recorder *rec);
int sheen_recording_info_get(const sheen_recorder *rec,sheen_recording_info *info);
void sheen_recording_close(sheen_recorder *rec);
#endif
