# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

A Commander X16 maze tank game written in C, compiled with the cc65 toolchain (`cl65`) targeting the cx16 platform.

## Build & Run

```sh
make xolo          # compile to build/xolo.prg
make distrib-xolo  # build and copy assets to distrib/
make run-xolo      # build, package, and launch in the X16 emulator
make clean         # remove build artifacts
```

The compiler is `cl65` (cc65 suite). When adding new `.c` source files, update the `xolo` recipe in `Makefile` to include them.

## Code Style

Google C style via `.clang-format`: 2-space indent, 130-column limit, braces attach, no tabs. Run `clang-format -i src/*.c src/*.h` to auto-format.

## Architecture

All sources live in `src/`. Entry point is `src/xolo.c` (`main`).

Use the wait() function in src/wait.c for vsync wait functionality.

**`src/vera-util.c/h`** — low-level VERA chip helpers:
- `load_into_vera()` loads binary asset files from the SD card into VRAM via CBM kernal calls.
- `uppercase_petscii_40x30()` switches to 40-column text mode.
- The header defines bitmask constants for VERA sprite/layer/tile register configuration (z-order, flip, BPP, dimensions, scale).

Assets (`.bin` files) live in `assets/` and are loaded at runtime into VRAM — they are not linked into the `.prg`. The `distrib/` directory is the run directory for the emulator; it must contain both `xolo.prg` and the asset binaries.

