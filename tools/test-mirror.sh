#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-mirror.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${sender_pid:-}" 2>/dev/null || true; wait "${sender_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sheen/mirror.h"
static int sendp(int fd,const uint8_t *p,size_t n,uint16_t port){struct sockaddr_in a={0};a.sin_family=AF_INET;a.sin_port=htons(port);a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);return sendto(fd,p,n,0,(struct sockaddr*)&a,sizeof(a))==(ssize_t)n?0:1;}
int main(void){uint16_t port=19095; sheen_mirror_receiver *r=sheen_mirror_open(port);if(!r)return 1;int s=socket(AF_INET,SOCK_DGRAM,0);if(s<0)return 2;
uint8_t one[17]={0x80,96,0,1,0,0,0,1,1,2,3,4,0x65,'S','H','E','E'}; if(sendp(s,one,sizeof(one),port))return 3; uint8_t out[1024]; sheen_mirror_packet_info i={0}; int n=sheen_mirror_receive(r,out,sizeof(out),&i); if(n!=8||out[4]!=0x65||memcmp(out+5,"SHEE",4)!=0)return 4;
uint8_t a[17]={0x80,96,0,2,0,0,0,2,1,2,3,4,0x7c,0x85,'A','B','C','D'}; uint8_t b[17]={0x80|0x00,96,0,3,0,0,0,2,1,2,3,4,0x45,'E','F','G','H'}; if(sendp(s,a,sizeof(a),port)||sendp(s,b,sizeof(b),port))return 5; n=sheen_mirror_receive(r,out,sizeof(out),&i); if(n!=13||out[4]!=0x65||memcmp(out+5,"ABCDEFGH",8)!=0)return 6;
close(s);sheen_mirror_close(r);puts("mirror ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/mirror/include" "$ROOT/casting/mirror/rtp-h264.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "mirror ok"
echo "Screen mirroring RTP/H264 tests passed"
