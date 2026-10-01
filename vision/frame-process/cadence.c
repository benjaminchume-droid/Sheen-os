#include <errno.h>
#include <stdint.h>
#include "sheen/frame-process.h"

int sheen_frame_cadence_init(sheen_frame_cadence *c,uint32_t num,uint32_t den){
    if(!c||!num||!den)return EINVAL;
    c->target_interval_ns=((uint64_t)den*1000000000ULL + num/2)/num;
    if(!c->target_interval_ns)return EINVAL;
    c->next_pts_ns=0;
    c->initialized=0;
    return 0;
}

int sheen_frame_cadence_step(sheen_frame_cadence *c,uint64_t pts,
                             sheen_frame_action *action,uint32_t *repeat){
    if(!c||!action||!repeat||!c->target_interval_ns)return EINVAL;
    *repeat=0;
    if(!c->initialized){
        c->initialized=1;
        c->next_pts_ns=pts+c->target_interval_ns;
        *action=SHEEN_FRAME_KEEP;
        return 0;
    }

    if(pts + c->target_interval_ns/2 < c->next_pts_ns){
        *action=SHEEN_FRAME_DROP;
        return 0;
    }

    uint64_t delta=pts>=c->next_pts_ns?pts-c->next_pts_ns:0;
    uint64_t intervals=(delta/c->target_interval_ns)+1;
    if(intervals>8) intervals=8;

    *action=intervals>1?SHEEN_FRAME_DUPLICATE:SHEEN_FRAME_KEEP;
    *repeat=intervals;
    c->next_pts_ns += intervals*c->target_interval_ns;
    return 0;
}
