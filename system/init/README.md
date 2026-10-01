Sheen service supervisor and boot target definitions.


## Phase 1.4

The native PID 1 implementation is `pid1.c`. It is compiled statically into the initramfs as `/init` and is responsible only for early process ownership and the development-shell handoff at this stage.
