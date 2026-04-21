# External Dependencies

This project keeps dependency setup simple by looking for local folders in `external/` first.

Recommended layout:

- `external/glfw/`
  - GLFW source tree containing `CMakeLists.txt`
- `external/glm/`
  - GLM folder containing `glm/glm.hpp`
- `external/glad/`
  - Generated GLAD files for **OpenGL 3.3 Core**
  - `include/glad/gl.h` or `include/glad/glad.h`
  - `include/KHR/khrplatform.h`
  - `src/gl.c` or `src/glad.c`
- `external/imgui/`
  - Dear ImGui source tree
  - Used only for the small terrain-control slider

Notes:

- GLFW will be built automatically by CMake when placed in `external/glfw`.
- GLM is header-only, so it only needs to be present in `external/glm`.
- Generate GLAD for:
  - Language: `C/C++`
  - API: `OpenGL`
  - Version: `3.3`
  - Profile: `Core`
- Dear ImGui is compiled directly from source and only needs to be present in `external/imgui`.

CMake also tries installed packages if these folders are missing, but the local folder layout above is the intended student-friendly setup.
