# Squid Game – Bridge Crossing Mini Game

A fun Raylib-based C++ mini-game inspired by the Squid Game bridge crossing challenge.  
Players control their character to cross a bridge while avoiding obstacles like planes.  
Includes clickable menu buttons, spaced UI text, and starts in a minimized window for convenience.

---

## 🎮 Features

- **Minimized Start** – Game starts in a non-fullscreen minimized window for better UX.
- **Responsive Buttons** – Menu buttons are lag-free and fully clickable.
- **Spaced UI Layout** – "Player" text, planes, and bridges have proper gaps to avoid overlap.
- **Clean Graphics** – Minimal clutter with better separation between elements.
- **Cross-Platform** – Works on macOS, Windows, and Linux.

---

## 🕹 Controls

| Action        | Key |
|--------------|-----|
| Move Left    | `A` or `Left Arrow` |
| Move Right   | `D` or `Right Arrow` |
| Select Button| Mouse Left Click |
| Exit Game    | `Esc` |

---

## 📦 Requirements

- **Raylib** (v4.5.0 or newer)
- **C++17** or newer
- A modern C++ compiler:
  - macOS: `clang++` (comes with Xcode Command Line Tools)
  - Windows: `MinGW` or MSVC
  - Linux: `g++`

---

## 🛠 Installation

### 1. Install Raylib

#### macOS
```bash
brew install raylib
#### Windows
```bash
choco install raylib
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libraylib-dev
```

### 2. Build & Run

#### macOS/Linux
```bash
g++ -std=c++17 main.cpp -o SquidGame -lraylib
./SquidGame
```

#### Windows (MinGW)
```bash
g++ -std=c++17 main.cpp -o SquidGame.exe -lraylib
SquidGame.exe
```

---

## 🖼 Screenshots

![Main Menu](screenshots/main_menu.png)
*Main menu with clickable buttons*

![Red Light Green Light](screenshots/doll.png)
*Bridge crossing gameplay*

![Player Crossing Bridge](screenshots/bridge.png)
*Player navigating the bridge obstacles*

![Dolgana Candy](screenshots/dolgana1.png)
*Game over screen with restart option*

![Dolgana Candy](screenshots/dolgana2.png)
*Victory screen after successful bridge crossing*


