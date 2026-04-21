#pragma once

#include "MeshData.h"

#include <glm/vec3.hpp>

#include <vector>

class MarchingCubes {
public:
    MarchingCubes(unsigned int width, unsigned int height, unsigned int depth, float cellSize, float isoLevel = 0.0f);

    const MeshData& getMeshData() const;
    float getMinHeight() const;
    float getMaxHeight() const;
    float getNoiseStrength() const;
    float getNoiseScale() const;
    float getBaseHeight() const;
    float getGroundLevel() const;
    unsigned int getSeed() const;

    void setNoiseStrength(float noiseStrength);
    void setNoiseScale(float noiseScale);
    void setBaseHeight(float baseHeight);
    void setGroundLevel(float groundLevel);
    void setSeed(unsigned int seed);
    void generateNewSeed();
    void regenerate();

private:
    struct GridPoint {
        glm::vec3 position;
        float density = 0.0f;
    };

    void buildScalarField();
    void buildMesh();
    std::size_t getGridIndex(unsigned int x, unsigned int y, unsigned int z) const;
    float sampleDensity(const glm::vec3& position) const;
    glm::vec3 interpolateVertex(const GridPoint& start, const GridPoint& end) const;
    void appendTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mDepth;
    float mCellSize;
    float mIsoLevel;
    float mNoiseStrength = 2.2f;
    float mNoiseScale = 0.10f;
    float mBaseHeight = -1.4f;
    float mGroundLevel = -3.4f;
    unsigned int mSeed = 1337u;
    float mMinHeight = 0.0f;
    float mMaxHeight = 0.0f;
    std::vector<GridPoint> mScalarField;
    MeshData mMeshData;
};
