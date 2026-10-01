#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-app-lifecycle.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { kill "${child_pid:-}" 2>/dev/null || true; wait "${child_pid:-}" 2>/dev/null || true; rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "sheen/app-lifecycle.h"
int main(void){
  sheen_app_manager *m=sheen_app_manager_create();if(!m)return 1;
  char *const argv[]={(char*)"sleep",(char*)"5",NULL};
  if(sheen_app_launch(m,"test-app","/bin/sleep",argv))return 2;
  sheen_app_info i={0};if(sheen_app_get(m,"test-app",&i)||i.state!=SHEEN_APP_RUNNING||i.pid<=0)return 3;
  if(sheen_app_pause(m,"test-app"))return 4;if(sheen_app_get(m,"test-app",&i)||i.state!=SHEEN_APP_PAUSED)return 5;
  if(sheen_app_resume(m,"test-app"))return 6;if(sheen_app_get(m,"test-app",&i)||i.state!=SHEEN_APP_RUNNING)return 7;
  if(sheen_app_stop(m,"test-app"))return 8;
  usleep(50000);sheen_app_reap(m);if(sheen_app_get(m,"test-app",&i)||i.state!=SHEEN_APP_STOPPED)return 9;
  puts("app lifecycle ok");sheen_app_manager_destroy(m);return 0;
}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/applications/lifecycle/app-lifecycle.c" "$tmp/test.c" -I"$ROOT/applications/lifecycle/include" -o "$tmp/test"
"$tmp/test" | grep -q "app lifecycle ok"
echo "Application lifecycle tests passed"
