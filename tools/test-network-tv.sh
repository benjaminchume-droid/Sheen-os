#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-network-tv.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${http_pid:-}" 2>/dev/null || true; kill "${udp_pid:-}" 2>/dev/null || true; wait "${http_pid:-}" 2>/dev/null || true; wait "${udp_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/streaming/include" -I"$ROOT/tv/live/include" -I"$ROOT/tv/network/include" "$ROOT/media/streaming/stream.c" "$ROOT/tv/live/live-session.c" "$ROOT/tv/network/network-tv.c" "$ROOT/tv/network/network-tv-cli.c" -o "$tmp/network"
cat > "$tmp/catalog.conf" <<EOF
iptv-1|Sheen HTTP|http://127.0.0.1:18081/live.ts
EOF
python3 - "$tmp" <<'PY' & http_pid=$!
import http.server, socketserver, sys, os
root=sys.argv[1]; os.chdir(root)
class H(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        body=b'NETWORK-TV-STREAM'
        self.send_response(200); self.send_header('Content-Length',str(len(body))); self.end_headers(); self.wfile.write(body)
    def log_message(self,*a): pass
with socketserver.TCPServer(('127.0.0.1',18081),H) as s: s.serve_forever()
PY
sleep 0.2
"$tmp/network" "$tmp/catalog.conf" list > "$tmp/list.json"
"$tmp/network" "$tmp/catalog.conf" open iptv-1 > "$tmp/open.json"
python3 - "$tmp" <<'PY'
import json,sys
r=sys.argv[1]
x=json.load(open(r+'/list.json',encoding='utf-8')); assert x['count']==1 and x['channels'][0]['channel_id']=='iptv-1'
y=json.load(open(r+'/open.json',encoding='utf-8')); assert y['channel_id']=='iptv-1' and y['first_read_bytes']>0
print('validated network TV catalog and live HTTP session')
PY
echo "Network TV tests passed"
