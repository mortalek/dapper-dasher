# Dapper Dasher

A small 2D side-scrolling game written in **C++**.

The goal is simple: run towards the finish line, avoid obstacles, and survive as long as possible.

![Dapper Dasher]

## 🎮 Gameplay

Dapper Dasher is a simple side-scroller focused on basic 2D game mechanics.

The player automatically moves through the level and must jump over obstacles and hazards to reach the finish line.

### Controls

| Key | Action |
|---|---|
| `SPACE` | Jump |
| `ESC` | Exit game |

## ✨ Features

- 2D side-scrolling gameplay
- Player movement and jumping
- Collision detection
- Obstacles and environmental hazards
- Sound effects and background audio
- Game loop and frame-based updates
- Texture and asset loading
- Win / lose conditions

## 🛠️ Built With

- **C++**
- **raylib**
- **Make**
- **Visual Studio Code**

## 📁 Project Structure

```text
Dapper-Dasher/
├── audio/              # Sound effects and music
├── textures/           # Game textures and sprites
├── dasher.cpp          # Main game implementation
├── Makefile            # Build configuration
├── main.code-workspace # VS Code workspace
└── README.md
```

## 🚀 Getting Started

### Prerequisites

Make sure you have:

- A C++ compiler
- `make`
- raylib
- Visual Studio Code (recommended)

### Build

Clone the repository:

```bash
git clone https://github.com/mortalek/dapper-dasher.git
cd dapper-dasher
```

Build the project:

```bash
make
```

Run the game:

```bash
./dasher
```

On Windows, run the generated executable:

```bash
dasher.exe
```

## 🧠 What I Learned

This project was created to improve my C++ and game development skills.

While working on Dapper Dasher, I practiced:

- C++ game programming
- Game loops and frame updates
- 2D movement and physics
- Collision detection
- Working with textures and audio
- Managing game state
- Using a C++ build system with Make
- Structuring a small game project

## 🎯 Future Improvements

Possible improvements include:

- [ ] Multiple levels
- [ ] More obstacle types
- [ ] Player animations
- [ ] Improved physics
- [ ] Score system
- [ ] High-score persistence
- [ ] Main menu
- [ ] Difficulty progression
- [ ] Additional sound effects and music

## 📄 License

This project is intended as a personal learning project.
