# Game

[Space Game](https://maiconspas.itch.io/spacegame)
# Name Options
Name: The Dying Echoes of Iara Sort: TDEI
Iara's Starfall Nomads
Iara's Lament: Voidfarers

# Inventory Game

![Build Linux Status](https://github.com/maiconpintoabreu/space-pixel-game/actions/workflows/linux.yml/badge.svg)
![Build Windows Status](https://github.com/maiconpintoabreu/space-pixel-game/actions/workflows/windows.yml/badge.svg)
![Build WebAssembly Status](https://github.com/maiconpintoabreu/space-pixel-game/actions/workflows/web.yml/badge.svg)
## Overview

Space Pixel Game is a simple game developed using C++ and Raylib.

## Features

- Star field simulation with stars that move and reset when they go offscreen.
- Basic game loop with update and render functions.
- Simple scoring system.
- Unit tests using GoogleTest.

## Project Structure

- `src/`: Contains the source code for the game.
    - `game/`: Contains the game logic and game manager.
    - `main/`: Contains the main entry point for the game.
- `.vscode/`: Contains Visual Studio Code settings and tasks.
- `.github/workflows/`: Contains GitHub Actions workflows for CI/CD.

## Building and Running

### Prerequisites

- CMake 22.0 or higher
- Make
- Raylib 5.5

### Build

```sh
make configure
make build
```

### Run

```sh
make run
```

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project is licensed under the MIT License.

Link to the license file: [LICENSE](LICENSE)
