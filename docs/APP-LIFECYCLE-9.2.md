# Phase 9.2 — Application Lifecycle

Phase 9.2 establishes the user-application process lifecycle below the public Sheen API.

Applications have explicit starting, running, paused, stopping, stopped, crashed, and failed states. The manager owns each process group so a stop operation targets the application's process tree rather than just one direct PID.

Pause/resume use real Linux process-control signals, and child exit status is reaped and converted into lifecycle state. Standard input/output/error are detached from the system console.

## Separation

This manager is distinct from the privileged system ServiceManager. System services remain governed by the service supervisor; user applications are managed through this lifecycle layer.

## Scope

Application permission checks, process namespaces, resource limits, package-to-process launch, Android application start, window/surface ownership, and crash restart policy remain later Phase 9 and Android compatibility stages.