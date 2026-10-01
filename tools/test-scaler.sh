#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-scaler.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sheen/scaler.h"
int main(void){
  uint8_t srcbuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  sheen_vision_frame src={.width=2,.height=2,.format=SHEEN_PIXEL_RGBA8888,.plane_count=1,.planes={{srcbuf,sizeof(srcbuf),8,2,2}},.pts=100};
  sheen_scaler *s=sheen_scaler_create(4,4,SHEEN_SCALE_NEAREST);if(!s)return 1;
  sheen_vision_frame dst={0};if(sheen_scaler_process(s,&src,&dst))return 2;if(dst.width!=4||dst.height!=4||dst.plane_count!=1)return 3;if(dst.planes[0].data[0]!=0||dst.planes[0].data[4]!=0||dst.planes[0].data[8]!=4)return 4;
  sheen_vision_frame_free(&dst);sheen_scaler_close(s);puts("scaler ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame/include" -I"$ROOT/vision/scaler/include" "$ROOT/vision/frame/frame-pipeline.c" "$ROOT/vision/scaler/scaler.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "scaler ok"
echo "Vision scaler tests passed"
