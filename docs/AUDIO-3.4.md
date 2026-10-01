# Phase 3.4 — Audio Engine Foundation

Phase 3.4 establishes the Linux audio hardware boundary that the later media/audio pipeline will consume.

The current primitive reads the live `/sys/class/sound` hierarchy through the Sheen HAL and reports actual ALSA sound cards and their bound drivers. It does not invent an audio device when the kernel exposes none.

Actual PCM sample scheduling, mixing, resampling, format conversion, and decoded-media audio output belong to the later Media Engine audio stages.

The probe is installed as `/usr/sbin/sheen-alsa-probe`.
