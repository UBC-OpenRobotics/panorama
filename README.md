# Panorama

A telemetry visualization client for robotics sensor data, developed by [UBC Open Robotics](https://openrobotics.ca). Panorama receives a live JSON stream from an ESP32 (or a mock Python server), and displays it in a GUI with real-time graphs and data logging.

## Table of Contents

- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Build & Run](#build--run)
- [JSON Data Format](#json-data-format)
- [Firmware](#firmware)
- [Project Structure](#project-structure)
- [CI](#ci)

## Architecture

```
ESP32 Firmware          Panorama Client (C++ / wxWidgets)
─────────────           ──────────────────────────────────
Sensor data        →    TCP Client  →  Message Model  →  GUI / Graphs
(JSON over TCP)         (port 3000)    DataBuffer         Data Logger
```

- **Client** (`client/`) — C++20 desktop app built with wxWidgets and wxMathPlot
- **Firmware** (`firmware/`) — ESP32 Arduino firmware that streams NDJSON sensor data over TCP (port 9000)
- **Tools** (`tools/`) — Python mock server and test utilities for development without hardware

## Prerequisites

### Client

| Platform | Requirements |
|----------|-------------|
| Linux    | `cmake >= 3.20`, `g++ (C++20)`, wxWidgets (`wx-config` on PATH) |
| macOS    | `cmake >= 3.20`, `clang (C++20)`, wxWidgets (`wx-config` on PATH) |
| Windows  | `cmake >= 3.20`, MSVC or MinGW, wxWidgets (set `WXWIN` environment variable) |

**Install wxWidgets (Linux/macOS):**
```bash
# Via the provided build script (recommended):
bash scripts/wxbuild.sh

# ** Other Install Methods ***
# Via Homebrew (macOS):
brew install wxwidgets

# Via apt (Ubuntu/Debian):
sudo apt-get install libwxgtk3.2-dev
```

### Firmware

- [PlatformIO](https://platformio.org/) CLI or IDE extension
- ESP32 dev board

## Build & Run

Source the environment script once per shell session to enable shorthand commands:

```bash
source scripts/env.sh
```

| Command    | Description                              |
|------------|------------------------------------------|
| `build`    | Configure and build the project          |
| `run`      | Build (if needed) and launch the client  |
| `clean`    | Remove the `build/` directory            |
| `runtests` | Build in Release mode and run CTest      |

Or use the scripts directly:

```bash
# Build
bash scripts/build.sh [Debug|Release]

# Run (connects to ESP32 at 127.0.0.1:3000)
bash scripts/run.sh

# Run with Python mock server instead of ESP32
bash scripts/run.sh -noesp # OR: -pserver

# Run in headless/console mode
bash scripts/run.sh -nogui

# Run tests
bash scripts/runtests.sh
```

## JSON Data Format

The client expects newline-delimited JSON (NDJSON). Each line is one sensor reading:

```json
{"datatype": "temperature", "data": 29.5, "dataunit": "celsius", "timestamp": 1770504589}
{"datatype": "light", "data": 0.2, "dataunit": "nm", "timestamp": 1770504589}
```

## Firmware

The ESP32 firmware creates a WiFi Access Point and streams sensor data over TCP.

**Flash with PlatformIO:**
```bash
cd firmware/panorama
pio run --target upload
pio device monitor
```

Update `upload_port` and `monitor_port` in `firmware/panorama/platformio.ini` to match your serial port (e.g., `/dev/ttyUSB0` on Linux, `COM3` on Windows).

## Project Structure

```
panorama/
├── client/
│   ├── src/            # C++ source files
│   ├── include/
│   │   ├── client/     # Client-specific headers
│   │   └── common/     # Shared headers (panorama_defines, panorama_utils)
│   └── external/       # wxMathPlot (bundled)
├── firmware/
│   └── panorama/       # PlatformIO ESP32 project
├── scripts/            # Build, run, test, and wxWidgets install scripts
├── tests/              # CTest unit tests
├── tools/              # Python mock server and dev utilities
├── rundir/             # Runtime data directory (config, logs, data)
└── doc/                # Documentation
```

## CI

CI runs on pull requests to `main` via GitHub Actions (Ubuntu). Pull requests prefixed with `WIP` skip CI automatically.
