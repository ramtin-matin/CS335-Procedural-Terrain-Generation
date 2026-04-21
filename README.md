# CS335-Procedural-Terrain-Generation

Procedural terrain graphics project in C++ and OpenGL 3.3 core.

Current coverage:

- OpenGL starter window
- Marching Cubes mesh extraction from a scalar field
- Procedural terrain density based on Perlin-style noise
- Live terrain controls for noise strength, noise scale, base height, and ground level
- Keyboard and mouse camera controls for exploring the terrain

## Project Layout

- `CMakeLists.txt` - build setup for the app and dependencies
- `include/Camera.h` / `src/Camera.cpp` - simple free-look camera for terrain exploration
- `include/MeshData.h` - shared mesh vertex/index container
- `include/MarchingCubes.h` / `src/MarchingCubes.cpp` - scalar field sampling and marching cubes mesh generation
- `include/Shader.h` / `src/Shader.cpp` - small shader loading and uniform helper
- `include/Terrain.h` / `src/Terrain.cpp` - grid mesh generation and terrain height sampling
- `src/main.cpp` - GLFW window setup, GLAD initialization, camera input, ImGui controls, and rendering loop
- `shaders/basic.vert` / `shaders/basic.frag` - simple shaders for terrain rendering
- `external/README.md` - expected layout for local GLFW, GLM, GLAD, and ImGui folders

## Dependency Setup

Place dependencies in `external/` using the layout described in [external/README.md](external/README.md).

This is the recommended setup because it avoids package managers.

## Build On macOS

1. Put `GLFW`, `GLM`, generated `GLAD`, and `Dear ImGui` in `external/`.
2. Configure the project:

```bash
cmake -S . -B build
```

3. Build it:

```bash
cmake --build build
```

4. Run it:

```bash
./build/procedural_terrain
```

If you use a multi-config generator, the executable may be under `build/Debug/` or `build/Release/`.

## Build On Windows

1. Put `GLFW`, `GLM`, generated `GLAD`, and `Dear ImGui` in `external/`.
2. Configure the project from a Developer Command Prompt or from a terminal with CMake available:

```powershell
cmake -S . -B build
```

3. Build it:

```powershell
cmake --build build --config Release
```

4. Run it:

```powershell
.\build\Release\procedural_terrain.exe
```

## Controls

- `W / A / S / D` move the camera
- Hold the right mouse button and move the mouse to look around
- Use the terrain sliders to rebuild the marching cubes terrain
- `Esc` closes the app.

## Terrain Density

The terrain surface comes from `MarchingCubes::sampleDensity()` in `src/MarchingCubes.cpp`.

- `terrainHeight` is built from broad low-frequency noise plus smaller high-frequency detail noise
- the terrain is grounded by blending that surface with a lower base plane
- the final density is the larger of:
  `terrainHeight - y` and `groundLevel - y`
- positive density means solid terrain
- negative density means empty space

If you want different terrain later, replace the logic inside `MarchingCubes::sampleDensity()` without changing the renderer or UI code.
