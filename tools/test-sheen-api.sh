#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-sheen-api.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${bus_pid:-}" "${svc_pid:-}" 2>/dev/null || true; wait "${bus_pid:-}" "${svc_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/ipc/ipc-bus.c" -o "$tmp/bus"
cat > "$tmp/service.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
int main(int argc,char **argv){int s=socket(AF_UNIX,SOCK_STREAM,0);struct sockaddr_un a={0};a.sun_family=AF_UNIX;strcpy(a.sun_path,argv[1]);connect(s,(struct sockaddr*)&a,sizeof(a));send(s,"{\"type\":\"register\",\"service\":\"api-test\",\"version\":\"1.0\"}\n",56,0);char b[8192];recv(s,b,sizeof(b),0);for(;;){int n=recv(s,b,sizeof(b)-1,0);if(n<=0)break;b[n]=0;if(strstr(b,"\"type\":\"request\"")){char *id=strstr(b,"\"request_id\":\"");char rid[128]={0};if(id){id+=14;char *e=strchr(id,'\"');if(e){size_t z=(size_t)(e-id);if(z>127)z=127;memcpy(rid,id,z);}}char out[1024];snprintf(out,sizeof(out),"{\"type\":\"response\",\"request_id\":\"%s\",\"status\":\"ok\",\"payload\":{\"value\":42}}\n",rid);send(s,out,strlen(out),0);send(s,"{\"type\":\"event\",\"event\":\"api.changed\",\"data\":{\"value\":42}}\n",70,0);}}return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$tmp/service.c" -o "$tmp/service"
"$tmp/bus" --socket "$tmp/bus.sock" & bus_pid=$!
sleep 0.05
"$tmp/service" "$tmp/bus.sock" & svc_pid=$!
sleep 0.05
cat > "$tmp/client.c" <<'EOF'
#include <stdio.h>
#include "sheen/api.h"
int main(int argc,char **argv){sheen_api_client *c=sheen_api_connect(argv[1],2000);if(!c)return 1;sheen_api_response r;if(sheen_api_request(c,"api-test","echo","{\"x\":1}",&r))return 2;if(r.status[0]==0||r.payload[0]==0)return 3;char e[4096];if(sheen_api_next_event(c,e,sizeof(e)))return 4;puts("sheen api ok");sheen_api_close(c);return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/sdk/runtime/include" "$ROOT/sdk/runtime/sheen-api.c" "$tmp/client.c" -o "$tmp/client"
"$tmp/client" "$tmp/bus.sock" | grep -q "sheen api ok"
echo "Public Sheen API integration tests passed"
