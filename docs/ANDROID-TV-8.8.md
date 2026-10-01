# Phase 8.8 — Android TV Behavior

Phase 8.8 establishes TV-specific interaction semantics above the generic Android input bridge.

The TV profile defines a 10-foot viewport policy, minimum focus-target sizing, remote repeat timing, and optional touch behavior. Android key codes for DPAD, Back, Home, Menu, media, channel, volume, mute, and power are translated into explicit TV actions.

Unknown key codes are rejected rather than assigned arbitrary TV behavior.

## Scope
Leanback framework APIs, focus-search integration with Android Views/Compose, TV provider channels, launcher/home semantics, system TV input APIs, and full Android TV package behavior remain later compatibility stages.