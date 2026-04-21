#include "Terrain.h"

#include <algorithm>
#include <cmath>
#include <limits>

Terrain::Terrain(unsigned int rows, unsigned int columns, float spacing, bool useHeightFunction)
    : mRows(rows),
      mColumns(columns),
      mSpacing(spacing),
      mUseHeightFunction(useHeightFunction) {
    buildMesh();
}

const MeshData& Terrain::getMeshData() const {
    return mMeshData;
}

float Terrain::getMinHeight() const {
    return mMinHeight;
}

float Terrain::getMaxHeight() const {
    return mMaxHeight;
}

void Terrain::buildMesh() {
    mMeshData.vertices.clear();
    mMeshData.indices.clear();

    mMeshData.vertices.reserve(static_cast<std::size_t>((mRows + 1) * (mColumns + 1)));
    mMeshData.indices.reserve(static_cast<std::size_t>(mRows * mColumns * 6));

    mMinHeight = std::numeric_limits<float>::max();
    mMaxHeight = std::numeric_limits<float>::lowest();

    const float halfWidth = static_cast<float>(mColumns) * mSpacing * 0.5f;
    const float halfDepth = static_cast<float>(mRows) * mSpacing * 0.5f;

    for (unsigned int row = 0; row <= mRows; ++row) {
        for (unsigned int column = 0; column <= mColumns; ++column) {
            const float x = static_cast<float>(column) * mSpacing - halfWidth;
            const float z = static_cast<float>(row) * mSpacing - halfDepth;
            const float y = sampleHeight(x, z);

            mMinHeight = std::min(mMinHeight, y);
            mMaxHeight = std::max(mMaxHeight, y);

            mMeshData.vertices.push_back({glm::vec3(x, y, z)});
        }
    }

    for (unsigned int row = 0; row < mRows; ++row) {
        for (unsigned int column = 0; column < mColumns; ++column) {
            const unsigned int topLeft = row * (mColumns + 1) + column;
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomLeft = (row + 1) * (mColumns + 1) + column;
            const unsigned int bottomRight = bottomLeft + 1;

            mMeshData.indices.push_back(topLeft);
            mMeshData.indices.push_back(bottomLeft);
            mMeshData.indices.push_back(topRight);

            mMeshData.indices.push_back(topRight);
            mMeshData.indices.push_back(bottomLeft);
            mMeshData.indices.push_back(bottomRight);
        }
    }

    if (mMeshData.vertices.empty()) {
        mMinHeight = 0.0f;
        mMaxHeight = 0.0f;
    }
}

float Terrain::sampleHeight(float x, float z) const {
    if (!mUseHeightFunction) {
        return 0.0f;
    }

    // Replace this method later with Perlin noise without touching the renderer.
    return 0.65f * std::sin(0.35f * x)
         + 0.45f * std::cos(0.30f * z)
         + 0.25f * std::sin(0.20f * (x + z));
}
