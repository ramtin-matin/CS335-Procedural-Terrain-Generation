#pragma once

#include "MeshData.h"

class Terrain {
public:
    Terrain(unsigned int rows, unsigned int columns, float spacing, bool useHeightFunction = true);

    const MeshData& getMeshData() const;
    float getMinHeight() const;
    float getMaxHeight() const;

private:
    void buildMesh();
    float sampleHeight(float x, float z) const;

    unsigned int mRows;
    unsigned int mColumns;
    float mSpacing;
    bool mUseHeightFunction;
    float mMinHeight = 0.0f;
    float mMaxHeight = 0.0f;
    MeshData mMeshData;
};
