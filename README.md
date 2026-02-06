# HeightmapRouting

## Requirements

- **CMake ≥ 3.5**
- **C++23 compatible compiler**

## Build

Tested on:

- Linux with **g++**
- Windows with **MinGW (g++)**

```bash
# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target HeightmapRouting --config Release -j5

# Run
./build/HeightmapRouting
```

## Controls

| Action                     | Key                             |
|----------------------------|---------------------------------|
| Move                       | Z, Q, S, D, Space, Left control |
| Toggle mouse lock (camera) | C                               |