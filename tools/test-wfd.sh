#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-wfd.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${server_pid:-}" 2>/dev/null || true; wait "${server_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -pthread -I"$ROOT/casting/wfd/include" "$ROOT/casting/wfd/wfd-rtsp.c" "$ROOT/casting/wfd/wfd-cli.c" -o "$tmp/wfd"
"$tmp/wfd" >/dev/null 2>"$tmp/server.err" & server_pid=$!
sleep 0.1
python3 - <<'PY'
import socket
s=socket.create_connection(('127.0.0.1',19554),timeout=2)
def tx(x):
    s.sendall(x.encode()); b=b''
    while b'\r\n\r\n' not in b: b+=s.recv(8192)
    return b.decode()
r=tx('OPTIONS rtsp://127.0.0.1/sheensink RTSP/1.0\r\nCSeq: 1\r\n\r\n'); assert '200 OK' in r and 'SETUP' in r
r=tx('GET_PARAMETER rtsp://127.0.0.1/sheensink RTSP/1.0\r\nCSeq: 2\r\n\r\n'); assert '200 OK' in r and 'wfd_video_formats' in r and 'wfd_client_rtp_ports' in r
r=tx('SETUP rtsp://127.0.0.1/sheensink RTSP/1.0\r\nCSeq: 3\r\n\r\n'); assert '200 OK' in r and 'server_port=19555' in r
r=tx('PLAY rtsp://127.0.0.1/sheensink RTSP/1.0\r\nCSeq: 4\r\n\r\n'); assert '200 OK' in r
r=tx('TEARDOWN rtsp://127.0.0.1/sheensink RTSP/1.0\r\nCSeq: 5\r\n\r\n'); assert '200 OK' in r
s.close(); print('validated WFD RTSP negotiation flow')
PY
echo "Wireless display RTSP tests passed"
