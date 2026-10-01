#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-streaming.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${http_pid:-}" 2>/dev/null || true; wait "${http_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/streaming/include" "$ROOT/media/streaming/stream.c" "$ROOT/media/streaming/stream-probe.c" -o "$tmp/probe"
printf "Sheen stream file fixture" > "$tmp/file.txt"
"$tmp/probe" "file://$tmp/file.txt" > "$tmp/file.json"
python3 - "$tmp" <<'PY'
import json,sys
x=json.load(open(sys.argv[1]+"/file.json"));assert x["bytes"]>0 and x["eof"]
PY
python3 - "$tmp" <<'PY' & http_pid=$!
import http.server,socketserver,sys,os
root=sys.argv[1];os.chdir(root)
class H(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        body=b"hello http stream"
        self.send_response(200);self.send_header("Content-Length",str(len(body)));self.end_headers();self.wfile.write(body)
    def log_message(self,*a): pass
with socketserver.TCPServer(("127.0.0.1",18080),H) as s:s.serve_forever()
PY
sleep 0.2
"$tmp/probe" "http://127.0.0.1:18080/file.txt" > "$tmp/http.json"
python3 - "$tmp" <<'PY'
import json,sys
x=json.load(open(sys.argv[1]+"/http.json"));assert x["bytes"]==17 and x["eof"]
PY
python3 - "$tmp" <<'PY' & udp_pid=$!
import socket,time,sys
time.sleep(.2);s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);s.sendto(b"udp stream",( "127.0.0.1",19090));s.close()
PY
"$tmp/probe" "udp://127.0.0.1:19090" > "$tmp/udp.json" & stream_pid=$!
wait "$udp_pid"; wait "$stream_pid"
python3 - "$tmp" <<'PY'
import json,sys
x=json.load(open(sys.argv[1]+"/udp.json"));assert x["bytes"]==10
PY
echo "Streaming file/HTTP/UDP tests passed"
