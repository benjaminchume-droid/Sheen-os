# Phase 9.4 — Application Sandboxing

Phase 9.4 establishes the Linux namespace sandbox boundary for user applications.

The sandbox can isolate mount, PID, UTS, and optionally network namespaces, applies `PR_SET_NO_NEW_PRIVS`, and can impose CPU and address-space limits before executing the application inside a configured root filesystem.

The process is created only after the sandbox root and executable are validated. Namespace execution is intentionally not treated as a successful CI result because those kernel operations may require elevated privileges on the host.

## Scope

Device-node filtering, seccomp syscall policy, cgroup resource enforcement, UID/GID mapping, read-only mount policy, app-specific bind mounts, and Android package launch integration remain later security/platform stages.