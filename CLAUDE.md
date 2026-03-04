# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Tetris clone (STC - Simple Tetris Clone) implemented in Unreal Engine 5. The core design philosophy is clean separation between platform-agnostic game logic and UE-specific rendering/input code.

## Building

This project uses Unreal Build Tool (UBT). Build via Visual Studio (open `Blocks.sln`) or from command line:

```bash
# Generate project files
Engine\Build\BatchFiles\GenerateProjectFiles.bat

# Build from command line
Engine\Build\BatchFiles\Build.bat Blocks Win64 Development
Engine\Build\BatchFiles\Build.bat BlocksEditor Win64 Development
```

The project targets UE 5.7 (engine association `"5.7"` in `Blocks.uproject`).

## Architecture

The codebase has three distinct layers:

**1. Platform Abstraction** ([Source/Blocks/stc/platform.hpp](Source/Blocks/stc/platform.hpp))
- Abstract interface (`stc::Platform`) for rendering, input, timing, and audio
- `ABlocksBlockGrid` implements this interface, bridging UE with core logic

**2. Core Game Logic** ([Source/Blocks/stc/game.cpp](Source/Blocks/stc/game.cpp) / [game.hpp](Source/Blocks/stc/game.hpp))
- Pure ANSI C++, completely platform-independent (~750 lines)
- All Tetris mechanics: piece spawning, collision, rotation (with wall-kick), line clearing, scoring, ghost piece
- 10×22 playfield, 7 tetromino types
- No global variables — `stc::Game` takes a `Platform*` reference

**3. Unreal Engine Integration** ([Source/Blocks/](Source/Blocks/))
- `ABlocksBlockGrid` — main game manager; spawns/destroys `ABlocksBlock` actors, plays audio, renders score text, implements `stc::Platform`
- `ABlocksBlock` — individual block actor with 7 color materials + shadow variants
- `ABlocksPawn` — translates UE input events to game commands
- `ABlocksPlayerController` — enables cursor/input
- `ABlocksGameMode` — wires up pawn and controller defaults

## Input Bindings

Input actions are defined in project settings (not in code). The pawn binds these action names:
- `OnLeft`, `OnRight`, `OnRotate`, `OnDown`, `OnDrop`

## Key Constants (in game.hpp)

- `BOARD_WIDTH = 10`, `BOARD_HEIGHT = 22`
- `INIT_DELAY_FALL = 1000` ms
- DAS: 200ms initial / 40ms repeat
- Scoring multipliers follow NES-style (×10): 400/1000/3000/12000 for 1-4 lines
