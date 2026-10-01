Long-running services expose stable IPC APIs to applications.


## Phase 2.4

`service-manager.c` is the native process supervisor. Service definitions are discovered from `/etc/sheen/services`, and status is published atomically under `/run/sheen/services/`.
