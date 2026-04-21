#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    unsigned int mProgramId = 0;

    static std::string readFile(const std::string& filePath);
    static unsigned int compileShader(unsigned int type, const std::string& source);
};
