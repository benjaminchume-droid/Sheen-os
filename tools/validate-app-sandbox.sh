#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/applications/sandbox/include/sheen/app-sandbox.h" ] || { echo "missing sandbox API" >&2; exit 1; }
[ -f "$ROOT/applications/sandbox/app-sandbox.c" ] || { echo "missing sandbox implementation" >&2; exit 1; }
grep -q "CLONE_NEWPID" "$ROOT/applications/sandbox/app-sandbox.c" || { echo "PID namespace missing" >&2; exit 1; }
grep -q "CLONE_NEWNET" "$ROOT/applications/sandbox/app-sandbox.c" || { echo "network namespace control missing" >&2; exit 1; }
grep -q "PR_SET_NO_NEW_PRIVS" "$ROOT/applications/sandbox/app-sandbox.c" || { echo "no-new-privs missing" >&2; exit 1; }
grep -q "RLIMIT_AS" "$ROOT/applications/sandbox/app-sandbox.c" || { echo "memory resource limit missing" >&2; exit 1; }
echo "Application sandbox contract valid"
