#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-superres.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include "sheen/superres.h"
int main(void){
 uint8_t srcbuf[16]={0,0,0,255,255,255,255,255,255,255,255,255,0,0,0,255};
 sheen_vision_frame src={.width=2,.height=2,.format=SHEEN_PIXEL_RGBA8888,.plane_count=1,.planes={{srcbuf,sizeof(srcbuf),8,2,2}},.pts=123,.duration=456};
 sheen_superres *s=sheen_superres_create(2);if(!s)return 1;sheen_vision_frame dst={0};if(sheen_superres_process(s,&src,&dst))return 2;if(dst.width!=4||dst.height!=4||dst.pts!=123||dst.duration!=456)return 3;if(dst.planes[0].data==0||dst.planes[0].size!=64)return 4;sheen_vision_frame_release(&dst);sheen_superres_close(s);puts("superres ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame/include" -I"$ROOT/vision/superres/include" "$ROOT/vision/frame/frame-pipeline.c" "$ROOT/vision/superres/classical-sr.c" "$tmp/test.c" -o "$tmp/test" -lm
"$tmp/test" | grep -q "superres ok"
echo "Classical super-resolution tests passed"
