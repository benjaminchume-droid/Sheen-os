# Phase 8.4 — Android Input Bridge

Phase 8.4 establishes the Linux evdev to Android input translation boundary.

The bridge translates physical Linux key events into Android-compatible key/action values and exposes absolute/touch coordinates for Android input surfaces. Common TV controls include DPAD, Home, Back, volume, power, media transport, channel, menu, numeric, and record keys.

Unknown Linux keys return an explicit unsupported result instead of being remapped arbitrarily. Touch coordinates remain raw device coordinates at this layer; display-space calibration and pointer routing belong to the graphics/input session layer.

## Scope

Multi-touch slot tracking, Android MotionEvent construction, controller/gamepad axes, keyboard text composition, remote repeat policy, focus/window routing, and Android TV-specific input dispatch remain later compatibility work.