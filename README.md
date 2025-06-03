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

## Features

- Full graphical interface
- Player display with name, role, coins, and status
- Detailed game log
- Notifications for illegal actions
- Full English support

## Credits

Created and designed by: <your-name-here> 