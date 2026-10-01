#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-vision-filters.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include "sheen/vision-frame.h"
#include "sheen/denoise.h"
#include "sheen/deblock.h"
#include "sheen/sharpen.h"
int main(void){
  uint8_t p[4*4*4]; for(int i=0;i<64;i++) p[i]=(uint8_t)(i*3);
  sheen_vision_frame f={.width=4,.height=4,.format=SHEEN_PIXEL_RGBA8888,.plane_count=1,.planes={{p,sizeof(p),16,4,4}},.pts=7};
  if(sheen_denoise_rgba(&f,32))return 1;
  if(sheen_deblock_rgba(&f,2,64))return 2;
  if(sheen_sharpen_rgba(&f,64))return 3;
  if(sheen_vision_frame_validate(&f))return 4;
  puts("filters ok"); return 0;
}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame/include" -I"$ROOT/vision/denoise/include" -I"$ROOT/vision/deblock/include" -I"$ROOT/vision/sharpen/include" "$ROOT/vision/frame/frame-pipeline.c" "$ROOT/vision/denoise/denoise-rgba.c" "$ROOT/vision/deblock/deblock-rgba.c" "$ROOT/vision/sharpen/sharpen-rgba.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "filters ok"
echo "Vision denoise/deblock/sharpen tests passed"
