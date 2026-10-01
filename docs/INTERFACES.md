# Sheen Interface Registry

These are the stable subsystem boundaries defined in Stage 0.1. Their concrete v1 semantic contracts now live under sdk/api/v1/.

| Boundary | v1 contract |
|---|---|
| HardwareManager | sdk/api/v1/hardware-manager.json |
| ServiceManager | sdk/api/v1/service-manager.json |
| IpcBus | sdk/api/v1/ipc-bus.json |
| PermissionManager | sdk/api/v1/permission-manager.json |
| SessionManager | sdk/api/v1/session-manager.json |
| DisplayManager | sdk/api/v1/display-manager.json |
| AudioManager | sdk/api/v1/audio-manager.json |
| MediaEngine | sdk/api/v1/media-engine.json |
| TvManager | sdk/api/v1/tv-manager.json |
| CastManager | sdk/api/v1/cast-manager.json |
| VisionEngine | sdk/api/v1/vision-engine.json |
| AndroidManager | sdk/api/v1/android-manager.json |
| StorageManager | sdk/api/v1/storage-manager.json |
| InputManager | sdk/api/v1/input-manager.json |

The contracts are semantic boundaries, not implementations. Concrete transport, ABI, driver, codec, compositor, and Android runtime choices remain implementation concerns.
