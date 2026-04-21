#include "Camera.h"
#include "MarchingCubes.h"
#include "Shader.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

namespace {

struct RenderMesh {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    int indexCount = 0;
    float minHeight = 0.0f;
    float maxHeight = 0.0f;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::Forward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::Backward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::Left, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::Right, deltaTime);
    }
}

RenderMesh uploadMesh(const MeshData& meshData, float minHeight, float maxHeight) {
    RenderMesh renderMesh;
    renderMesh.indexCount = static_cast<int>(meshData.indices.size());
    renderMesh.minHeight = minHeight;
    renderMesh.maxHeight = maxHeight;

    glGenVertexArrays(1, &renderMesh.vao);
    glGenBuffers(1, &renderMesh.vbo);
    glGenBuffers(1, &renderMesh.ebo);

    glBindVertexArray(renderMesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderMesh.vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.vertices.size() * sizeof(Vertex)),
        meshData.vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderMesh.ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.indices.size() * sizeof(unsigned int)),
        meshData.indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return renderMesh;
}

void destroyMesh(RenderMesh& renderMesh) {
    if (renderMesh.ebo != 0) {
        glDeleteBuffers(1, &renderMesh.ebo);
        renderMesh.ebo = 0;
    }

    if (renderMesh.vbo != 0) {
        glDeleteBuffers(1, &renderMesh.vbo);
        renderMesh.vbo = 0;
    }

    if (renderMesh.vao != 0) {
        glDeleteVertexArrays(1, &renderMesh.vao);
        renderMesh.vao = 0;
    }
}

void rebuildMesh(RenderMesh& renderMesh, const MeshData& meshData, float minHeight, float maxHeight) {
    destroyMesh(renderMesh);
    renderMesh = uploadMesh(meshData, minHeight, maxHeight);
}

glm::mat4 buildProjectionMatrix(GLFWwindow* window, float verticalFieldOfView) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    const float aspectRatio = height == 0 ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    return glm::perspective(glm::radians(verticalFieldOfView), aspectRatio, 0.1f, 100.0f);
}

void processMouseLook(GLFWwindow* window, Camera& camera, bool& firstMouse, double& lastMouseX, double& lastMouseY) {
    const bool lookingAround = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
        && !ImGui::GetIO().WantCaptureMouse;

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (!lookingAround) {
        firstMouse = true;
        return;
    }

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    const float xOffset = static_cast<float>(mouseX - lastMouseX);
    const float yOffset = static_cast<float>(lastMouseY - mouseY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    camera.processMouseMovement(xOffset, yOffset);
}

void renderScene(const Shader& shader, const RenderMesh& mesh, const glm::mat4& model,
                 const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition,
                 const glm::vec3& skyBottomColor, const glm::vec3& skyTopColor) {
    shader.use();
    shader.setMat4("uModel", model);
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    shader.setVec3("uCameraPosition", cameraPosition);
    shader.setBool("uUseHeightColoring", true);
    shader.setFloat("uMinHeight", mesh.minHeight);
    shader.setFloat("uMaxHeight", mesh.maxHeight);
    shader.setVec3("uBaseColor", glm::vec3(0.24f, 0.58f, 0.31f));
    shader.setVec3("uSkyBottomColor", skyBottomColor);
    shader.setVec3("uSkyTopColor", skyTopColor);
    shader.setFloat("uFogNear", 12.0f);
    shader.setFloat("uFogFar", 42.0f);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void renderSky(const Shader& shader, unsigned int skyVao, const glm::vec3& skyBottomColor, const glm::vec3& skyTopColor) {
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    shader.use();
    shader.setVec3("uSkyBottomColor", skyBottomColor);
    shader.setVec3("uSkyTopColor", skyTopColor);

    glBindVertexArray(skyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

}  // namespace

int main() {
    try {
        if (glfwInit() == GLFW_FALSE) {
            std::cerr << "Failed to initialize GLFW.\n";
            return EXIT_FAILURE;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow* window = glfwCreateWindow(1280, 720, "Procedural Marching Cubes Terrain", nullptr, nullptr);
        if (window == nullptr) {
            std::cerr << "Failed to create a GLFW window.\n";
            glfwTerminate();
            return EXIT_FAILURE;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSwapInterval(1);

        if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0) {
            std::cerr << "Failed to initialize GLAD.\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        glEnable(GL_DEPTH_TEST);

        const std::string shaderDirectory = SHADER_DIR;
        Shader shader(shaderDirectory + "/basic.vert", shaderDirectory + "/basic.frag");
        Shader skyShader(shaderDirectory + "/sky.vert", shaderDirectory + "/sky.frag");

        unsigned int skyVao = 0;
        glGenVertexArrays(1, &skyVao);

        Camera camera(glm::vec3(0.0f, 3.5f, 11.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -14.0f);

        MarchingCubes marchingCubes(40, 24, 40, 0.35f, 0.0f);
        float noiseStrength = marchingCubes.getNoiseStrength();
        float noiseScale = marchingCubes.getNoiseScale();
        float baseHeight = marchingCubes.getBaseHeight();
        float groundLevel = marchingCubes.getGroundLevel();
        unsigned int terrainSeed = marchingCubes.getSeed();
        bool wireframeEnabled = false;
        RenderMesh terrainMesh = uploadMesh(
            marchingCubes.getMeshData(),
            marchingCubes.getMinHeight(),
            marchingCubes.getMaxHeight()
        );

        float lastFrameTime = static_cast<float>(glfwGetTime());
        bool firstMouse = true;
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;

        while (glfwWindowShouldClose(window) == GLFW_FALSE) {
            glfwPollEvents();
            const float currentFrameTime = static_cast<float>(glfwGetTime());
            const float deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            processInput(window, camera, deltaTime);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            processMouseLook(window, camera, firstMouse, lastMouseX, lastMouseY);

            ImGui::SetNextWindowPos(ImVec2(18.0f, 18.0f), ImGuiCond_Once);
            ImGui::Begin("Terrain Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            bool terrainChanged = false;
            bool seedChanged = false;
            terrainChanged |= ImGui::SliderFloat("Noise Strength", &noiseStrength, 0.20f, 4.50f, "%.2f");
            terrainChanged |= ImGui::SliderFloat("Noise Scale", &noiseScale, 0.03f, 0.35f, "%.3f");
            terrainChanged |= ImGui::SliderFloat("Base Height", &baseHeight, -4.0f, 2.0f, "%.2f");
            terrainChanged |= ImGui::SliderFloat("Ground Level", &groundLevel, -4.0f, -0.5f, "%.2f");

            if (ImGui::Button("Regenerate Terrain")) {
                marchingCubes.generateNewSeed();
                terrainSeed = marchingCubes.getSeed();
                seedChanged = true;
            }

            ImGui::SameLine();
            ImGui::Text("Seed: %u", terrainSeed);

            ImGui::Checkbox("Wireframe", &wireframeEnabled);
            ImGui::Separator();
            ImGui::Text("Triangles: %d", terrainMesh.indexCount / 3);
            ImGui::TextDisabled("W/A/S/D move");
            ImGui::TextDisabled("Hold right mouse button to look around");
            ImGui::End();

            if (terrainChanged || seedChanged) {
                marchingCubes.setNoiseStrength(noiseStrength);
                marchingCubes.setNoiseScale(noiseScale);
                marchingCubes.setBaseHeight(baseHeight);
                marchingCubes.setGroundLevel(groundLevel);
                marchingCubes.regenerate();
                rebuildMesh(
                    terrainMesh,
                    marchingCubes.getMeshData(),
                    marchingCubes.getMinHeight(),
                    marchingCubes.getMaxHeight()
                );
            }

            int displayWidth = 0;
            int displayHeight = 0;
            glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
            glViewport(0, 0, displayWidth, displayHeight);
            const glm::vec3 skyBottomColor(0.77f, 0.88f, 0.95f);
            const glm::vec3 skyTopColor(0.30f, 0.54f, 0.78f);
            glClearColor(skyBottomColor.r, skyBottomColor.g, skyBottomColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            renderSky(skyShader, skyVao, skyBottomColor, skyTopColor);
            glPolygonMode(GL_FRONT_AND_BACK, wireframeEnabled ? GL_LINE : GL_FILL);

            renderScene(
                shader,
                terrainMesh,
                glm::mat4(1.0f),
                camera.getViewMatrix(),
                buildProjectionMatrix(window, camera.getZoom()),
                camera.getPosition(),
                skyBottomColor,
                skyTopColor
            );

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        destroyMesh(terrainMesh);
        glDeleteVertexArrays(1, &skyVao);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
