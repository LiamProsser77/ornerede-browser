# Ornerede Browser

Ornerede is an open-source web browser that's private and built with C++.

It is designed to be installed and launched through terminal commands.

## Features

- C++ application
- Terminal-based installation
- Cross-platform goals

## Supported Platforms

- Linux
- Android (Termux)
- Windows
- macOS

## Engine
Ornerede has no engine it runs on it's own code, which makes it lightweight and easy to run.

## Building

Requirements:
- C++ compiler
- CMake

```bash
cmake -S . -B build
cmake --build build
```

## Running

```bash
./build/ornerede
```

## License

Apache License 2.0
