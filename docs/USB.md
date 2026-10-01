# USB-first design

The base system is read-mostly and compressed. Writable overlays, caches and user data are separated to reduce flash wear. The same image should be portable across supported x86-64 machines while selecting drivers and acceleration at runtime.
