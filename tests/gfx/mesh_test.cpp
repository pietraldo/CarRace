#include <gtest/gtest.h>
#include "gfx/Mesh.h"
#include "../test_utils/test_helpers.h"

// ============================================================================
// Vertex Structure Tests
// ============================================================================

class VertexTest : public ::testing::Test {};

TEST_F(VertexTest, Vertex_DefaultInitialization) {
    Vertex vertex;
    // Vertex should be default constructible
    SUCCEED();
}

TEST_F(VertexTest, Vertex_PositionAssignment) {
    Vertex vertex;
    vertex.Position = glm::vec3(1.0f, 2.0f, 3.0f);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 2.0f, 3.0f), vertex.Position);
}

TEST_F(VertexTest, Vertex_NormalAssignment) {
    Vertex vertex;
    vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 1.0f, 0.0f), vertex.Normal);
}

TEST_F(VertexTest, Vertex_TexCoordsAssignment) {
    Vertex vertex;
    vertex.TexCoords = glm::vec2(0.5f, 0.5f);
    EXPECT_NEAR(vertex.TexCoords.x, 0.5f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(vertex.TexCoords.y, 0.5f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(VertexTest, Vertex_TangentAssignment) {
    Vertex vertex;
    vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 0.0f, 0.0f), vertex.Tangent);
}

TEST_F(VertexTest, Vertex_BitangentAssignment) {
    Vertex vertex;
    vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 0.0f, 1.0f), vertex.Bitangent);
}

TEST_F(VertexTest, Vertex_BoneIDsArray) {
    Vertex vertex;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_BoneIDs[i] = i;
    }
    
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        EXPECT_EQ(i, vertex.m_BoneIDs[i]);
    }
}

TEST_F(VertexTest, Vertex_WeightsArray) {
    Vertex vertex;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_Weights[i] = 0.25f * i;
    }
    
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        EXPECT_NEAR(0.25f * i, vertex.m_Weights[i], TestHelpers::FLOAT_EPSILON);
    }
}

TEST_F(VertexTest, Vertex_MaxBoneInfluence) {
    EXPECT_EQ(MAX_BONE_INFLUENCE, 4);
}

TEST_F(VertexTest, Vertex_CompleteInitialization) {
    Vertex vertex;
    vertex.Position = glm::vec3(1.0f, 2.0f, 3.0f);
    vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertex.TexCoords = glm::vec2(0.5f, 0.5f);
    vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
    
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 2.0f, 3.0f), vertex.Position);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 1.0f, 0.0f), vertex.Normal);
}

// ============================================================================
// Texture Structure Tests
// ============================================================================

class TextureTest : public ::testing::Test {};

TEST_F(TextureTest, Texture_DefaultInitialization) {
    Texture texture;
    SUCCEED();
}

TEST_F(TextureTest, Texture_IDAssignment) {
    Texture texture;
    texture.id = 42;
    EXPECT_EQ(42u, texture.id);
}

TEST_F(TextureTest, Texture_TypeAssignment) {
    Texture texture;
    texture.type = "texture_diffuse";
    EXPECT_EQ("texture_diffuse", texture.type);
}

TEST_F(TextureTest, Texture_PathAssignment) {
    Texture texture;
    texture.path = "assets/textures/test.png";
    EXPECT_EQ("assets/textures/test.png", texture.path);
}

TEST_F(TextureTest, Texture_DiffuseType) {
    Texture texture;
    texture.type = "texture_diffuse";
    EXPECT_EQ("texture_diffuse", texture.type);
}

TEST_F(TextureTest, Texture_SpecularType) {
    Texture texture;
    texture.type = "texture_specular";
    EXPECT_EQ("texture_specular", texture.type);
}

TEST_F(TextureTest, Texture_NormalType) {
    Texture texture;
    texture.type = "texture_normal";
    EXPECT_EQ("texture_normal", texture.type);
}

TEST_F(TextureTest, Texture_HeightType) {
    Texture texture;
    texture.type = "texture_height";
    EXPECT_EQ("texture_height", texture.type);
}

// ============================================================================
// MirrorSide Enum Tests
// ============================================================================

class MirrorSideTest : public ::testing::Test {};

TEST_F(MirrorSideTest, MirrorSide_LeftValue) {
    MirrorSide side = MirrorSide::Left;
    EXPECT_EQ(MirrorSide::Left, side);
}

TEST_F(MirrorSideTest, MirrorSide_RightValue) {
    MirrorSide side = MirrorSide::Right;
    EXPECT_EQ(MirrorSide::Right, side);
}

TEST_F(MirrorSideTest, MirrorSide_NotEqual) {
    EXPECT_NE(MirrorSide::Left, MirrorSide::Right);
}

TEST_F(MirrorSideTest, MirrorSide_Assignment) {
    MirrorSide side1 = MirrorSide::Left;
    MirrorSide side2 = side1;
    EXPECT_EQ(side1, side2);
}

// ============================================================================
// Normal Vector Tests
// ============================================================================

class NormalVectorTest : public ::testing::Test {
protected:
    glm::vec3 CalculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        return glm::normalize(glm::cross(edge1, edge2));
    }
};

TEST_F(NormalVectorTest, Normal_TriangleXY) {
    glm::vec3 v1(0.0f, 0.0f, 0.0f);
    glm::vec3 v2(1.0f, 0.0f, 0.0f);
    glm::vec3 v3(0.0f, 1.0f, 0.0f);
    
    glm::vec3 normal = CalculateNormal(v1, v2, v3);
    
    // Normal should point in +Z direction
    EXPECT_NEAR(normal.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.z, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(NormalVectorTest, Normal_TriangleXZ) {
    glm::vec3 v1(0.0f, 0.0f, 0.0f);
    glm::vec3 v2(1.0f, 0.0f, 0.0f);
    glm::vec3 v3(0.0f, 0.0f, 1.0f);
    
    glm::vec3 normal = CalculateNormal(v1, v2, v3);
    
    // Normal should point in -Y direction
    EXPECT_NEAR(normal.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.y, -1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(NormalVectorTest, Normal_TriangleYZ) {
    glm::vec3 v1(0.0f, 0.0f, 0.0f);
    glm::vec3 v2(0.0f, 1.0f, 0.0f);
    glm::vec3 v3(0.0f, 0.0f, 1.0f);
    
    glm::vec3 normal = CalculateNormal(v1, v2, v3);
    
    // Normal should point in +X direction
    EXPECT_NEAR(normal.x, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(normal.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(NormalVectorTest, Normal_Normalized) {
    glm::vec3 v1(0.0f, 0.0f, 0.0f);
    glm::vec3 v2(2.0f, 0.0f, 0.0f);
    glm::vec3 v3(0.0f, 3.0f, 0.0f);
    
    glm::vec3 normal = CalculateNormal(v1, v2, v3);
    
    float length = glm::length(normal);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Tangent and Bitangent Tests
// ============================================================================

class TangentBitangentTest : public ::testing::Test {
protected:
    void CalculateTangentBitangent(
        const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
        glm::vec3& tangent, glm::vec3& bitangent)
    {
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;
        
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);
        
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);
    }
};

TEST_F(TangentBitangentTest, TangentBitangent_SimpleTriangle) {
    glm::vec3 pos1(0.0f, 0.0f, 0.0f);
    glm::vec3 pos2(1.0f, 0.0f, 0.0f);
    glm::vec3 pos3(0.0f, 1.0f, 0.0f);
    
    glm::vec2 uv1(0.0f, 0.0f);
    glm::vec2 uv2(1.0f, 0.0f);
    glm::vec2 uv3(0.0f, 1.0f);
    
    glm::vec3 tangent, bitangent;
    CalculateTangentBitangent(pos1, pos2, pos3, uv1, uv2, uv3, tangent, bitangent);
    
    // Tangent and bitangent should be normalized
    EXPECT_NEAR(glm::length(tangent), 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glm::length(bitangent), 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(TangentBitangentTest, TangentBitangent_Orthogonal) {
    glm::vec3 pos1(0.0f, 0.0f, 0.0f);
    glm::vec3 pos2(1.0f, 0.0f, 0.0f);
    glm::vec3 pos3(0.0f, 1.0f, 0.0f);
    
    glm::vec2 uv1(0.0f, 0.0f);
    glm::vec2 uv2(1.0f, 0.0f);
    glm::vec2 uv3(0.0f, 1.0f);
    
    glm::vec3 tangent, bitangent;
    CalculateTangentBitangent(pos1, pos2, pos3, uv1, uv2, uv3, tangent, bitangent);
    
    // Tangent and bitangent should be orthogonal
    float dot = glm::dot(tangent, bitangent);
    EXPECT_NEAR(dot, 0.0f, 0.01f); // Slightly larger epsilon for numerical stability
}

// ============================================================================
// Texture Coordinate Tests
// ============================================================================

class TextureCoordinateTest : public ::testing::Test {};

TEST_F(TextureCoordinateTest, TexCoords_Origin) {
    glm::vec2 uv(0.0f, 0.0f);
    EXPECT_NEAR(uv.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(uv.y, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(TextureCoordinateTest, TexCoords_TopRight) {
    glm::vec2 uv(1.0f, 1.0f);
    EXPECT_NEAR(uv.x, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(uv.y, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(TextureCoordinateTest, TexCoords_Center) {
    glm::vec2 uv(0.5f, 0.5f);
    EXPECT_NEAR(uv.x, 0.5f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(uv.y, 0.5f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(TextureCoordinateTest, TexCoords_Wrapping) {
    // Test texture wrapping (values > 1.0)
    glm::vec2 uv(2.5f, 3.7f);
    EXPECT_NEAR(uv.x, 2.5f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(uv.y, 3.7f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Vertex Array Tests
// ============================================================================

class VertexArrayTest : public ::testing::Test {};

TEST_F(VertexArrayTest, VertexArray_EmptyVector) {
    std::vector<Vertex> vertices;
    EXPECT_EQ(0u, vertices.size());
}

TEST_F(VertexArrayTest, VertexArray_SingleVertex) {
    std::vector<Vertex> vertices;
    Vertex v;
    v.Position = glm::vec3(1.0f, 2.0f, 3.0f);
    vertices.push_back(v);
    
    EXPECT_EQ(1u, vertices.size());
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 2.0f, 3.0f), vertices[0].Position);
}

TEST_F(VertexArrayTest, VertexArray_MultipleVertices) {
    std::vector<Vertex> vertices;
    for (int i = 0; i < 10; i++) {
        Vertex v;
        v.Position = glm::vec3(i, i, i);
        vertices.push_back(v);
    }
    
    EXPECT_EQ(10u, vertices.size());
    EXPECT_VEC3_EQ(glm::vec3(5.0f, 5.0f, 5.0f), vertices[5].Position);
}

// ============================================================================
// Index Array Tests
// ============================================================================

class IndexArrayTest : public ::testing::Test {};

TEST_F(IndexArrayTest, IndexArray_Triangle) {
    std::vector<unsigned int> indices = {0, 1, 2};
    EXPECT_EQ(3u, indices.size());
    EXPECT_EQ(0u, indices[0]);
    EXPECT_EQ(1u, indices[1]);
    EXPECT_EQ(2u, indices[2]);
}

TEST_F(IndexArrayTest, IndexArray_Quad) {
    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    EXPECT_EQ(6u, indices.size());
}

TEST_F(IndexArrayTest, IndexArray_TriangleCount) {
    std::vector<unsigned int> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    size_t triangleCount = indices.size() / 3;
    EXPECT_EQ(3u, triangleCount);
}
