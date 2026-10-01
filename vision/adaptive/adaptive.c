#include <errno.h>
#include <limits.h>
#include <string.h>
#include "sheen/adaptive.h"

static int compatible(const sheen_adaptive_context *c,const sheen_adaptive_stage *s) {
    return s->enabled && (s->required_capabilities & ~c->capabilities) == 0;
}

int sheen_adaptive_add_stage(sheen_adaptive_context *c,const sheen_adaptive_stage *s) {
    if(!c||!s||!s->id||!s->id[0]||c->stage_count>=SHEEN_ADAPTIVE_MAX_STAGES)
        return EINVAL;
    c->stages[c->stage_count++]=*s;
    return 0;
}

int sheen_adaptive_plan(const sheen_adaptive_context *c,sheen_adaptive_plan *out) {
    if(!c||!out)return EINVAL;
    memset(out,0,sizeof(*out));

    size_t order[SHEEN_ADAPTIVE_MAX_STAGES];
    size_t count=0;
    for(size_t i=0;i<c->stage_count;i++) if(compatible(c,&c->stages[i]))
        order[count++]=i;

    for(size_t i=0;i<count;i++)
        for(size_t j=i+1;j<count;j++) {
            const sheen_adaptive_stage *a=&c->stages[order[i]];
            const sheen_adaptive_stage *b=&c->stages[order[j]];
            uint32_t as=(a->quality>=c->quality_target)?a->quality-c->quality_target:c->quality_target-a->quality;
            uint32_t bs=(b->quality>=c->quality_target)?b->quality-c->quality_target:c->quality_target-b->quality;
            if((bs<as)|| (bs==as&&b->cost_us<a->cost_us)) {
                size_t x=order[i];order[i]=order[j];order[j]=x;
            }
        }

    for(size_t k=0;k<count;k++) {
        const sheen_adaptive_stage *s=&c->stages[order[k]];
        uint64_t next_cost=(uint64_t)out->estimated_cost_us+s->cost_us;
        uint64_t next_latency=(uint64_t)out->estimated_latency_us+s->latency_us;
        uint32_t next_quality=out->estimated_quality+s->quality;
        if(c->latency_budget_us && next_latency>c->latency_budget_us) continue;
        if(out->selected_count>=SHEEN_ADAPTIVE_MAX_SELECTED) break;
        out->selected_indices[out->selected_count++]=order[k];
        out->estimated_cost_us=(uint32_t)(next_cost>UINT_MAX?UINT_MAX:next_cost);
        out->estimated_latency_us=(uint32_t)(next_latency>UINT_MAX?UINT_MAX:next_latency);
        out->estimated_quality=next_quality;
    }
    return 0;
}
