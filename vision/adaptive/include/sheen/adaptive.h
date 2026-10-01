#ifndef SHEEN_ADAPTIVE_H
#define SHEEN_ADAPTIVE_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_ADAPTIVE_MAX_STAGES 64
#define SHEEN_ADAPTIVE_MAX_SELECTED 64

enum {
    SHEEN_REQ_CPU = 1ULL << 0,
    SHEEN_REQ_GPU = 1ULL << 1,
    SHEEN_REQ_HDR = 1ULL << 2,
    SHEEN_REQ_ALPHA = 1ULL << 3
};

typedef struct {
    char id[64];
    uint64_t required_capabilities;
    uint32_t quality;
    uint32_t cost_us;
    uint32_t latency_us;
    int enabled;
} sheen_adaptive_stage;

typedef struct {
    uint64_t capabilities;
    uint32_t quality_target;
    uint32_t latency_budget_us;
    sheen_adaptive_stage stages[SHEEN_ADAPTIVE_MAX_STAGES];
    size_t stage_count;
} sheen_adaptive_context;

typedef struct {
    size_t selected_count;
    size_t selected_indices[SHEEN_ADAPTIVE_MAX_SELECTED];
    uint32_t estimated_cost_us;
    uint32_t estimated_latency_us;
    uint32_t estimated_quality;
} sheen_adaptive_plan;

int sheen_adaptive_add_stage(sheen_adaptive_context *context,
                             const sheen_adaptive_stage *stage);
int sheen_adaptive_plan(const sheen_adaptive_context *context,
                        sheen_adaptive_plan *plan);
#endif
