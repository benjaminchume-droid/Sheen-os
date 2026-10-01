#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-hdr.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include "sheen/hdr.h"
int main(void){
  uint16_t y16[4]={4096,4096,4096,4096};
  uint16_t uv16[4]={32768,32768,32768,32768};
  uint8_t *y=(uint8_t*)y16,*uv=(uint8_t*)uv16;
  sheen_vision_frame src={0};
  src.width=2;src.height=2;src.format=SHEEN_PIXEL_P010;src.plane_count=2;
  src.planes[0]=(sheen_frame_plane){y,sizeof(y16),4,2,2};
  src.planes[1]=(sheen_frame_plane){uv,sizeof(uv16),4,2,1};
  src.pts=99;src.duration=40;
  sheen_vision_frame dst={0};
  if(sheen_hdr_p010_to_sdr_rgba(&src,&dst,SHEEN_TRANSFER_PQ,SHEEN_HDR_RANGE_FULL,100.0f)) return 1;
  if(dst.width!=2||dst.height!=2||dst.format!=SHEEN_PIXEL_RGBA8888||dst.pts!=99||dst.duration!=40)return 2;
  if(!dst.planes[0].data || dst.planes[0].size!=16)return 3;
  if(sheen_hdr_p010_to_sdr_rgba(&src,&dst,SHEEN_TRANSFER_HLG,SHEEN_HDR_RANGE_FULL,100.0f)!=95)return 4;
  sheen_vision_frame_release(&dst);
  puts("hdr ok");
  return 0;
}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame/include" -I"$ROOT/vision/hdr/include" "$ROOT/vision/frame/frame-pipeline.c" "$ROOT/vision/hdr/pq-p010.c" "$tmp/test.c" -o "$tmp/test" -lm
"$tmp/test" | grep -q "hdr ok"
echo "HDR/SDR processing tests passed"
