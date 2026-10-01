#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-media-cast.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${http_pid:-}" 2>/dev/null || true; wait "${http_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/protocols/include" "$ROOT/casting/protocols/upnp-media.c" "$ROOT/casting/protocols/media-cast-cli.c" -o "$tmp/cast"
cat > "$tmp/server.py" <<'PY'
from http.server import BaseHTTPRequestHandler,HTTPServer
calls=[]
class H(BaseHTTPRequestHandler):
    def do_GET(self):
        xml='''<?xml version="1.0"?><root><device><serviceList><service><serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType><controlURL>/upnp/control</controlURL></service></serviceList></device></root>'''.encode()
        self.send_response(200);self.send_header('Content-Length',str(len(xml)));self.end_headers();self.wfile.write(xml)
    def do_POST(self):
        n=int(self.headers.get('Content-Length','0'));body=self.rfile.read(n);calls.append((self.headers.get('SOAPAction',''),body))
        self.send_response(200);resp=b'<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body/></s:Envelope>';self.send_header('Content-Length',str(len(resp)));self.end_headers();self.wfile.write(resp)
    def log_message(self,*a):pass
HTTPServer(('127.0.0.1',18082),H).serve_forever()
PY
python3 "$tmp/server.py" & http_pid=$!; sleep 0.2
"$tmp/cast" "http://127.0.0.1:18082/device.xml" "http://media.example/live.ts" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8')); assert x['state']=='playing' and x['control_url'].endswith('/upnp/control')
print('validated UPnP AVTransport SetAVTransportURI + Play flow')
PY
echo "Media casting tests passed"
