#include "Camera.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
    : mPosition(position),
      mFront(0.0f, 0.0f, -1.0f),
      mWorldUp(up),
      mYaw(yaw),
      mPitch(pitch) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

const glm::vec3& Camera::getPosition() const {
    return mPosition;
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    const float velocity = mMovementSpeed * deltaTime;

    if (direction == CameraMovement::Forward) {
        mPosition += mFront * velocity;
    }
    if (direction == CameraMovement::Backward) {
        mPosition -= mFront * velocity;
    }
    if (direction == CameraMovement::Left) {
        mPosition -= mRight * velocity;
    }
    if (direction == CameraMovement::Right) {
        mPosition += mRight * velocity;
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mMouseSensitivity;
    yOffset *= mMouseSensitivity;

    mYaw += xOffset;
    mPitch += yOffset;

    if (constrainPitch) {
        if (mPitch > 89.0f) {
            mPitch = 89.0f;
        }
        if (mPitch < -89.0f) {
            mPitch = -89.0f;
        }
    }

    updateCameraVectors();
}

float Camera::getZoom() const {
    return mZoom;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = std::cos(glm::radians(mYaw)) * std::cos(glm::radians(mPitch));
    front.y = std::sin(glm::radians(mPitch));
    front.z = std::sin(glm::radians(mYaw)) * std::cos(glm::radians(mPitch));

    mFront = glm::normalize(front);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}
