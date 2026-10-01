# Phase 2.4 — Service Manager

Phase 2.4 establishes the native process supervisor for Sheen system services.

## Service definitions

Services are declarative configuration files under `/etc/sheen/services`. Each definition names an executable, enabled state, and restart policy. The manager discovers `.conf` files instead of hard-coding service names.

## Responsibilities

- discover service definitions
- launch enabled service processes
- redirect service stdio away from the controlling console
- reap exited children
- apply restart-on-failure policy
- terminate managed processes during shutdown
- publish an atomic JSON Lines status file

The status publication is an internal runtime artifact; the public ServiceManager and IPC APIs remain the long-term interface.

## Current service

`configs/services/device-manager.conf` installs the real Phase 2.3 device manager as a supervised service.

## Scope

IPC transport, permission enforcement, service activation dependencies, resource limits, and production isolation are later runtime/security stages.
