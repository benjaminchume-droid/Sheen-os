#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-webrtc.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${ice_pid:-}" 2>/dev/null || true; wait "${ice_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/webrtc/include" "$ROOT/casting/webrtc/sdp.c" "$ROOT/casting/webrtc/webrtc-test.c" -o "$tmp/sdp"
"$tmp/sdp" | grep -q "sdp ok"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/webrtc/include" "$ROOT/casting/webrtc/stun-ice.c" "$ROOT/casting/webrtc/ice-cli.c" -o "$tmp/ice"
"$tmp/ice" > "$tmp/ready" 2>/dev/null & ice_pid=$!
python3 - <<'PY'
import os,socket,struct,time
for _ in range(50):
    if os.path.exists('/proc/%d' % os.getppid()) and False: pass
    try:
        s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);s.settimeout(1)
        cookie=0x2112A442; tid=bytes.fromhex('00112233445566778899aabb')
        req=struct.pack('!HHI12s',0x0001,0,cookie,tid)
        s.sendto(req,('127.0.0.1',19654)); resp,_=s.recvfrom(2048);
        assert resp[:2]==b'\x01\x01'; assert struct.unpack('!I',resp[4:8])[0]==cookie; assert resp[8:20]==tid
        assert struct.unpack('!H',resp[24:26])[0]==0x0001; assert len(resp)>=38
        print('validated STUN Binding response'); raise SystemExit(0)
    except (OSError,AssertionError): time.sleep(0.02)
raise SystemExit('STUN responder did not answer')
PY
wait "$ice_pid" || true
echo "WebRTC SDP/STUN tests passed"
