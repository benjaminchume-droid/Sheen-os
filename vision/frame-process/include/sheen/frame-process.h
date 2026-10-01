#ifndef SHEEN_FRAME_PROCESS_H
#define SHEEN_FRAME_PROCESS_H
#include <stdint.h>
typedef enum {
    SHEEN_FRAME_KEEP,
    SHEEN_FRAME_DROP,
    SHEEN_FRAME_DUPLICATE
} sheen_frame_action;
typedef struct {
    uint64_t target_interval_ns;
    uint64_t next_pts_ns;
    int initialized;
} sheen_frame_cadence;
int sheen_frame_cadence_init(sheen_frame_cadence *cadence,uint32_t fps_num,uint32_t fps_den);
int sheen_frame_cadence_step(sheen_frame_cadence *cadence,uint64_t pts_ns,
                            sheen_frame_action *action,uint32_t *repeat_count);
#endif
