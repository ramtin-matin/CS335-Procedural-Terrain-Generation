#pragma once

#include <glm/vec3.hpp>

#include <vector>

struct Vertex {
    glm::vec3 position;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
