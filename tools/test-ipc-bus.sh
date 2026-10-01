#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() {  if [ -n "${pid:-}" ]; then kill "$pid" 2>/dev/null || true; wait "$pid" 2>/dev/null || true; fi; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/ipc/ipc-bus.c" -o "$tmp/sheen-ipc-bus"
"$tmp/sheen-ipc-bus" --socket "$tmp/bus.sock" & pid=$!
python3 - "$tmp/bus.sock" <<'PY'
import json, socket, sys, time
sock_path=sys.argv[1]
for _ in range(50):
    try: socket.socket(socket.AF_UNIX).connect(sock_path); break
    except OSError: time.sleep(0.02)

class Conn:
    def __init__(self):
        self.s=socket.socket(socket.AF_UNIX,socket.SOCK_STREAM); self.s.connect(sock_path); self.buf=b""
    def send(self,obj): self.s.sendall((json.dumps(obj,separators=(",",":"))+"\n").encode())
    def recv(self):
        while b"\n" not in self.buf:
            chunk=self.s.recv(65536); assert chunk; self.buf+=chunk
        line,self.buf=self.buf.split(b"\n",1); return json.loads(line)

service=Conn(); client=Conn()
service.send({"type":"register","service":"test-service","version":"1.0","capabilities":["test.echo"]})
assert service.recv()["type"]=="registered"
client.send({"type":"discover"})
d=client.recv(); assert d["status"]=="ok"; assert any(x["service"]=="test-service" for x in d["payload"])
client.send({"type":"negotiate","service":"test-service","version":"1.0"})
n=client.recv(); assert n["payload"]["accepted_version"]=="1.0"
client.send({"type":"request","service":"test-service","operation":"echo","request_id":"req-1","payload":{"value":"hello"},"deadline_ms":2000})
q=service.recv(); assert q["type"]=="request"; assert q["request_id"]=="req-1"; assert q["operation"]=="echo"
service.send({"type":"response","request_id":"req-1","status":"ok","payload":{"echo":"hello"}})
r=client.recv(); assert r["request_id"]=="req-1" and r["status"]=="ok"; assert r["payload"]["echo"]=="hello"
service.send({"type":"event","event":"test.changed","version":"1.0","timestamp":"2026-10-01T00:00:00Z","data":{"value":1}})
e=client.recv(); assert e["type"]=="event" and e["event"]=="test.changed"
print("validated IPC discover/negotiate/request/response/event flow")
PY
echo "IPC bus integration tests passed"
