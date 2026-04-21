#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

enum class CameraMovement {
    Forward,
    Backward,
    Left,
    Right
};

class Camera {
public:
    Camera(
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
        const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );

    glm::mat4 getViewMatrix() const;
    const glm::vec3& getPosition() const;

    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    float getZoom() const;

private:
    void updateCameraVectors();

    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mWorldUp;
    float mYaw;
    float mPitch;
    float mMovementSpeed = 7.5f;
    float mMouseSensitivity = 0.10f;
    float mZoom = 45.0f;
};
