#include "Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexSource = readFile(vertexPath);
    const std::string fragmentSource = readFile(fragmentPath);

    const unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    mProgramId = glCreateProgram();
    glAttachShader(mProgramId, vertexShader);
    glAttachShader(mProgramId, fragmentShader);
    glLinkProgram(mProgramId);

    int success = 0;
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        int logLength = 0;
        glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> infoLog(static_cast<std::size_t>(logLength));
        glGetProgramInfoLog(mProgramId, logLength, nullptr, infoLog.data());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(mProgramId);

        throw std::runtime_error("Failed to link shader program:\n" + std::string(infoLog.data()));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (mProgramId != 0) {
        glDeleteProgram(mProgramId);
    }
}

void Shader::use() const {
    glUseProgram(mProgramId);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(mProgramId, name.c_str()), static_cast<int>(value));
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(mProgramId, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(mProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(mProgramId, name.c_str()), 1, glm::value_ptr(value));
}

std::string Shader::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    const unsigned int shader = glCreateShader(type);
    const char* sourcePointer = source.c_str();

    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> infoLog(static_cast<std::size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
        glDeleteShader(shader);

        throw std::runtime_error("Failed to compile shader:\n" + std::string(infoLog.data()));
    }

    return shader;
}
