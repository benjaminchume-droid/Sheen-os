#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-package.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/build_apk.py" <<'PY'
import struct,zipfile,sys
out=sys.argv[1]
strings=['manifest','package','com.example.sheen','uses-feature','name','android.software.leanback']
data=b''; offsets=[]
for s in strings:
    b=s.encode(); offsets.append(len(data)); data += bytes([len(s),len(b)])+b+b'\0'
header_size=28; strings_start=header_size+4*len(strings)
sp=struct.pack('<HHIIIIII',1,header_size,strings_start+len(data),len(strings),0,0x100,strings_start,0)+b''.join(struct.pack('<I',x) for x in offsets)+data
def start(name_idx,attrs):
    attr_start=20; attr_size=20; count=len(attrs); size=36+20*count
    node=struct.pack('<HHIIII',0x0102,36,size,1,0,0)
    ext=struct.pack('<IIHHHHHH',0xffffffff,name_idx,attr_start,attr_size,count,0,0,0)
    arr=b''
    for nidx,vidx in attrs:
        arr += struct.pack('<IIIIHBBI',0xffffffff,nidx,vidx,8,0,3,0,vidx)
    return node+ext+arr
chunks=[]
chunks.append(start(0,[(1,2)]))
chunks.append(start(3,[(4,5)]))
xml=struct.pack('<HHI',3,8,8+len(sp)+sum(len(x) for x in chunks))+sp+b''.join(chunks)
with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED) as z:
    z.writestr('AndroidManifest.xml',xml)
    z.writestr('classes.dex',b'dex\n035\0'+b'0'*32)
PY
python3 "$tmp/build_apk.py" "$tmp/test.apk"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/android/package/include" "$ROOT/android/package/package-manager.c" "$ROOT/android/package/package-cli.c" -o "$tmp/apk" -lsqlite3 -lz -ldl -lpthread -lm
"$tmp/apk" inspect "$tmp/test.apk" > "$tmp/inspect.json"
python3 - "$tmp/inspect.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert x['package_id']=='com.example.sheen' and x['has_manifest'] and x['has_dex'] and x['has_tv_feature']
print('validated APK manifest/package/Dex/TV feature inspection')
PY
"$tmp/apk" install "$tmp/packages.db" "$tmp/packages" "$tmp/test.apk" > "$tmp/install.json"
python3 - "$tmp/install.json" <<'PY'
import json,sys,os
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert x['package_id']=='com.example.sheen'
assert os.path.exists(x['installed_path'])
print('validated transactional APK staging')
PY
"$tmp/apk" count "$tmp/packages.db" "$tmp/packages" | grep -q '"count":1'
echo "Android APK package manager tests passed"
