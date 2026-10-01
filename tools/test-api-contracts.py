#!/usr/bin/env python3
"""Validate the structural invariants of Sheen v1 API contracts."""
import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
API = ROOT / "sdk" / "api" / "v1"
EXPECTED = ["common.json","hardware-manager.json","service-manager.json","ipc-bus.json","permission-manager.json","session-manager.json","display-manager.json","audio-manager.json","media-engine.json","tv-manager.json","cast-manager.json","vision-engine.json","android-manager.json","storage-manager.json","input-manager.json"]

def fail(message):
    print(f"API contract validation failed: {message}", file=sys.stderr)
    raise SystemExit(1)

for name in EXPECTED:
    path = API / name
    if not path.is_file():
        fail(f"missing {path.relative_to(ROOT)}")
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path.name}: invalid JSON: {exc}")
    if not isinstance(data, dict):
        fail(f"{path.name}: root must be an object")
    if "$id" not in data or not data["$id"].startswith("sheen://api/v1/"):
        fail(f"{path.name}: invalid v1 $id")
    if name != "common.json":
        for key in ("title","operations","events"):
            if key not in data: fail(f"{path.name}: missing {key}")
        if not isinstance(data["operations"], dict) or not data["operations"]:
            fail(f"{path.name}: operations must be a non-empty object")
        if not isinstance(data["events"], list):
            fail(f"{path.name}: events must be an array")
        for operation, definition in data["operations"].items():
            if not isinstance(definition, dict) or "request" not in definition or "response" not in definition:
                fail(f"{path.name}: operation {operation} must define request and response")

print(f"API contract validation passed: {len(EXPECTED)} v1 contracts")
