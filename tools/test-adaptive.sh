#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-adaptive.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include "sheen/adaptive.h"
int main(void){sheen_adaptive_context c={0};c.capabilities=SHEEN_REQ_CPU|SHEEN_REQ_GPU;c.quality_target=80;c.latency_budget_us=500;
sheen_adaptive_stage a={"scale",SHEEN_REQ_CPU,70,100,150,1};
sheen_adaptive_stage b={"superres",SHEEN_REQ_GPU,100,250,250,1};
sheen_adaptive_stage d={"hdr",SHEEN_REQ_HDR,100,50,50,1};
if(sheen_adaptive_add_stage(&c,&a)||sheen_adaptive_add_stage(&c,&b)||sheen_adaptive_add_stage(&c,&d))return 1;
sheen_adaptive_plan p={0};if(sheen_adaptive_build_plan(&c,&p))return 2;
if(p.selected_count!=2)return 3;
if(strcmp(c.stages[p.selected_indices[0]].id,"scale"))return 4;
if(p.estimated_latency_us>500)return 5;
puts("adaptive ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/adaptive/include" "$ROOT/vision/adaptive/adaptive.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "adaptive ok"
echo "Adaptive Vision planner tests passed"
