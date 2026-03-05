# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-based controller that automates a Westinghouse WGen12000DF generator with a Generac 200-amp ATS (Automatic Transfer Switch). Detects utility power loss via optocouplers, starts the generator, transfers load, and reverses the process when utility returns. Includes a web UI for monitoring and manual control.

## Build System

This is a **PlatformIO** project with three environments:

- **`Native`** — Native unit tests using Unity framework (runs on host machine, not ESP32)
- **`featheresp32`** — Adafruit Huzzah32 ESP32 Feather target
- **`adafruit_feather_esp32_v2`** — Alternate ESP32 Feather V2 target

### Common Commands

Build for ESP32: `pio run -e featheresp32`
Upload to ESP32: `pio run -e featheresp32 -t upload`
Upload + Serial Monitor: `pio run -e featheresp32 -t upload -t monitor`
Upload web files (SPIFFS): `pio run -e featheresp32 -t uploadfs`
Run native unit tests: `pio test -e Native`
Serial monitor: `pio device monitor -b 115200`

### Required Environment Variables

Set these before building ESP32 targets:
- `ENV_WIFI_SSID` / `ENV_WIFI_PW` — WiFi credentials
- `ENV_LOG_SERVER_NAME` / `ENV_LOG_SERVER_PORT` — Syslog server (optional, enables remote logging)

### Native Test Prerequisites

Install MinGW and add its `bin` folder to PATH. Tests use the Unity framework and compile with the `PIO_UNIT_TESTING` define, which gates out Arduino/ESP32 code.

## Architecture

### State Machine (core pattern)

The system is driven by a **state machine** in `src/States/Orchestration.cpp`. `Orchestration` is the central class — it owns devices, pins, queues, and the state map. States implement `IState` (GetName/DoAction). Events are defined in `src/States/IEvent.cpp` as the `Event` enum.

**State flow:** Utility_Off → wait/confirm → Generator_Start → Generator_On → warm up → Transfer_To_Generator → Idle. Reverse: Utility_On → wait/confirm → Transfer_To_Utility → Generator_Stop → cool down → Idle.

Sub-events (e.g., Generator_Starting, Generator_Warm_Up) don't have their own state classes — they're handled within the parent state's DoAction.

### Concurrency Model (FreeRTOS tasks)

`main.cpp` creates three FreeRTOS tasks:
1. **Pin change listener** (priority 3) — `WaitAndListenForPinChanges()` dequeues pin change events, debounces, and triggers state changes
2. **State change processor** (priority 2) — `WaitAndListenForStateChanges()` dequeues state events and calls DoAction on the appropriate state
3. **Web server** (priority 1) — serves the web UI and API endpoints

Hardware interrupts feed pin changes into a queue; state transitions go through a separate queue. Both use `IQueue<T>` (RTOS-backed `RtosQueue` on device, `SimpleQueue` in tests).

### Hardware Abstraction

- `IO/IBoardIo.h` — interface for GPIO/timing operations
- `IO/RtosIO.cpp` — real ESP32 implementation
- `IO/MockBoard.cpp` — mock for native unit tests

### Key Directories

- `src/States/` — State machine: Orchestration, individual states, Event enum
- `src/Devices/` — PowerDevice (utility), StartableDevice (generator), TransferSwitch
- `src/IO/` — Hardware abstraction, pins, queues, serial, WiFi, ring buffer
- `src/SimpleWeb/` — Web server: Router + Controllers (state/data/index endpoints)
- `data/` — SPIFFS filesystem (index.html web UI served to browser)
- `test/` — Native unit tests; `test_main.cpp` is the entry point
- `src/config.h` — Timing constants (warm-up, cool-down, debounce, retry counts)

### Code Style Notes

- `.cpp` files are used as both implementation and header (included directly via `#include`), with `#pragma once` guards. There are no separate `.h` files for most classes.
- `PIO_UNIT_TESTING` preprocessor guard in `main.cpp` excludes Arduino code during native tests.
- GPIO pin assignments are configured in `main.cpp` (not config.h).
