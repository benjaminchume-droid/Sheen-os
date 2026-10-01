#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-media-library.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" -I"$ROOT/media/library/include" "$ROOT/media/demux/container.c" "$ROOT/media/library/media-library.c" "$ROOT/media/library/media-library-scan.c" -o "$tmp/index" -lsqlite3 -ldl -lpthread -lm
mkdir -p "$tmp/media/subdir"
printf "\001\002\003\004ftypisom" > "$tmp/media/movie.mp4"
printf "OggSreal" > "$tmp/media/song.ogg"
"$tmp/index" "$tmp/library.db" "$tmp/media" > "$tmp/result.json"
python3 - "$tmp" <<'PY'
import json,sqlite3,sys
r=sys.argv[1];x=json.load(open(r+"/result.json",encoding="utf-8"))
assert x["indexed"]==2
db=sqlite3.connect(r+"/library.db")
rows=db.execute("select source,container from media order by source").fetchall()
assert len(rows)==2 and rows[0][1] in ("isobmff","ogg") and rows[1][1] in ("isobmff","ogg")
ids=db.execute("select id from media").fetchall();assert all(len(x[0])==32 for x in ids)
db.close();print("validated persistent media catalog: 2 items")
PY
echo "Media library scan/persistence tests passed"
