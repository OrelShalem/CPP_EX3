# Coup - Card Game

A strategic card game with SFML-based graphical interface.

## System Requirements

- C++17 or newer
- SFML 2.5 or newer
- Make

## Installing SFML

### Ubuntu/Debian:
```bash
sudo apt-get install libsfml-dev
```

### Fedora:
```bash
sudo dnf install SFML-devel
```

### macOS (Homebrew):
```bash
brew install sfml
```

### Windows:
Download SFML from the official website: https://www.sfml-dev.org/download.php
and configure the libraries as required.

## Build Instructions

1. Clone the repository:
```bash
git clone <repo-url>
cd Coup
```

2. Build the project:
```bash
make
```

## Running

To run the game:
```bash
./bin/Main
```

Select `g` to launch the GUI interface, or `c` for console mode.

## Using the GUI Interface

1. Click on a player to select them
2. Click on an action you want to perform:
   - gather: collect coins
   - tax: collect tax
   - bribe: pay a bribe
   - arrest: arrest another player
   - sanction: place sanctions on another player
   - coup: perform a coup against another player

For actions that require a target (arrest, sanction, coup) - you must first select a target player.

Press the Escape key to cancel the current selection.

## Game Details

Coup is a strategic game where each player has a special role and unique abilities. The goal is to be the last player standing.

Roles:
- General: Expert in coups and blocking them
- Governor: Can cancel tax actions
- Spy: Can block arrests
- Baron: Expert in economic investments
- Judge: Can cancel bribe actions
- Merchant: Expert in trade matters

## Project Structure

### Main Directories:
- `src/`: Source code files
- `src/Roles/`: Implementation of different game roles
- `assets/`: Graphical resources (fonts)
- `test/`: Unit test files
- `bin/`: Compiled binary files
- `obj/`: Compiled object files

### Core Classes:

#### Game.hpp/cpp
The core game class that manages game flow, game state, and allowed actions. It is responsible for:
- Turn management
- Game rules enforcement
- Handling player actions
- Managing overall game state

#### Player.hpp/cpp
Represents a player in the game with:
- Name and role
- Coin count
- Status (active/eliminated)
- Special abilities according to their role

#### Role Classes (in Roles/ directory)
Each role is represented by a separate class implementing its unique abilities:
- `General.hpp/cpp`: General role with coup abilities
- `Governor.hpp/cpp`: Governor role with tax cancellation abilities
- `Spy.hpp/cpp`: Spy role with arrest blocking abilities
- `Baron.hpp/cpp`: Baron role with economic abilities
- `Judge.hpp/cpp`: Judge role with bribe cancellation abilities
- `Merchant.hpp/cpp`: Merchant role with trade abilities

#### CoupGUI.hpp/cpp
Implementation of the graphical user interface using SFML:
- Game board display
- User interaction handling
- Animations and information display
- Interface with the game engine

#### GameExceptions.hpp
Definitions of game-specific exceptions used for error handling and edge cases.

### Core Functionality

#### Game Modes
- Full graphical interface with SFML
- Console mode for testing and development

#### Build Structure
Comprehensive Makefile with capabilities for:
- Compiling the main project
- Compiling and running unit tests
- Memory leak checks with Valgrind

## Testing

The project includes a comprehensive test suite:
- Unit tests for various classes
- Edge case tests for special scenarios
- Integration tests for complete game flow

To run the tests:
```bash
make test
```

## Memory Checks

You can check for memory leaks using Valgrind:
```bash
make valgrind
```

## Features

- Full graphical interface
- Player display with name, role, coins, and status
- Detailed game log
- Notifications for illegal actions
- Full English support 