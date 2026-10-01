#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-app-sandbox.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include "sheen/app-sandbox.h"
int main(void){sheen_sandbox_config c={0};snprintf(c.rootfs,sizeof(c.rootfs),"/");snprintf(c.executable,sizeof(c.executable),"/bin/sh");c.network_enabled=1;c.memory_limit_bytes=512ULL*1024ULL*1024ULL;c.cpu_limit_seconds=5;if(sheen_sandbox_verify(&c))return 1;sheen_sandbox *s=sheen_sandbox_create(&c);if(!s)return 2;sheen_sandbox_destroy(s);puts("sandbox config ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/applications/sandbox/app-sandbox.c" "$tmp/test.c" -I"$ROOT/applications/sandbox/include" -o "$tmp/test"
"$tmp/test" | grep -q "sandbox config ok"
echo "Application sandbox configuration tests passed"
