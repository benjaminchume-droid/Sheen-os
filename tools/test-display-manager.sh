#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-display-manager.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){ kill "${display_pid:-}" "${bus_pid:-}" 2>/dev/null || true; wait "${display_pid:-}" "${bus_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/ipc/ipc-bus.c" -o "$tmp/bus"
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/hardware/graphics/drm-probe.c" -o "$tmp/drm"
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/display/display-manager.c" -o "$tmp/display"
"$tmp/bus" --socket "$tmp/bus.sock" & bus_pid=$!
python3 - "$tmp/bus.sock" "$tmp/drm" "$tmp/display" <<'PY' & test_pid=$!
import os,socket,subprocess,sys,time,json
bus,probe,display=sys.argv[1:]
def wait_socket(p):
    for _ in range(100):
        if os.path.exists(p): return
        time.sleep(0.01)
    raise RuntimeError("bus socket did not appear")
wait_socket(bus)
p=subprocess.Popen([display,"--bus",bus,"--probe",probe])
class C:
    def __init__(self): self.s=socket.socket(socket.AF_UNIX,socket.SOCK_STREAM); self.s.connect(bus); self.buf=b""
    def send(self,x): self.s.sendall((json.dumps(x,separators=(",",":"))+"\n").encode())
    def recv(self):
        while b"\n" not in self.buf: self.buf+=self.s.recv(65536)
        line,self.buf=self.buf.split(b"\n",1); return json.loads(line)
c=C()
for _ in range(100):
    c.send({"type":"discover","service":"display-manager"}); d=c.recv()
    if d.get("payload"): break
    time.sleep(0.02)
assert any(x["service"]=="display-manager" for x in d["payload"])
c.send({"type":"request","service":"display-manager","operation":"get_displays","request_id":"display-1","payload":{},"deadline_ms":2000})
r=c.recv(); assert r["request_id"]=="display-1" and r["status"] in ("ok","error")
if r["status"]=="ok": assert isinstance(r["payload"],dict) and "cards" in r["payload"]
print("validated IPC-backed display manager and live DRM probe")
p.terminate(); p.wait(timeout=2)
PY
wait "$test_pid"
echo "Display manager integration tests passed"
