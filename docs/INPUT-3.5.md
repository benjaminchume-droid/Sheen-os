# Phase 3.5 — Input Engine

Phase 3.5 establishes real Linux input-event discovery for remotes, keyboards, mice, gamepads, and other evdev sources.

The probe enumerates `/sys/class/input/event*`, opens the corresponding `/dev/input/event*` device when accessible, and queries device identity plus event capabilities using Linux `EVIOCGNAME`, `EVIOCGID`, and `EVIOCGBIT`.

Input is reported from the kernel and driver state at runtime. No virtual remote, keyboard, or controller is created by the implementation.

The probe is installed as `/usr/sbin/sheen-evdev-probe`. Higher-level input sessions and remote-first navigation are later runtime/shell stages.
