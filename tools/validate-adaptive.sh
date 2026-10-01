#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/adaptive/include/sheen/adaptive.h" ] || { echo "missing adaptive Vision API" >&2; exit 1; }
[ -f "$ROOT/vision/adaptive/adaptive.c" ] || { echo "missing adaptive planner" >&2; exit 1; }
grep -q "sheen_adaptive_build_plan" "$ROOT/vision/adaptive/adaptive.c" || { echo "planner entry point missing" >&2; exit 1; }
grep -q "required_capabilities" "$ROOT/vision/adaptive/adaptive.c" || { echo "capability gating missing" >&2; exit 1; }
grep -q "latency_budget_us" "$ROOT/vision/adaptive/adaptive.c" || { echo "latency budget missing" >&2; exit 1; }
echo "Adaptive Vision planner contract valid"
