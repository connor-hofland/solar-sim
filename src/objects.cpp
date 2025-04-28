#include "objects.hpp"
#include "structs.hpp"
#include "shaders.hpp"
unsigned int shaderProgram;
Objects::Objects(std::vector<float> init_vertices, std::vector<unsigned int> init_indices, Point init_acceleration, Point init_velocity, Point init_offset)
    : vertices(init_vertices), indices(init_indices), acceleration(init_acceleration), velocity(init_velocity), offset(init_offset) {}

void Objects::initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Objects::render() {
    glUseProgram(shaderProgram);

    int offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    glUniform3f(offsetLoc, offset.x, offset.y, offset.z);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Objects::update() {
    velocity.x += acceleration.x;
    velocity.y += acceleration.y;
    velocity.z += acceleration.z;
    offset.x += velocity.x;
    offset.y += velocity.y;
    offset.z += velocity.z;
}

void Objects::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
std::vector<float> generateCubeVertices(float cx, float cy, float cz, float r) {
    std::vector<float> vertices = {
        // x, y, z
        cx - r, cy - r, cz - r, // 0
        cx + r, cy - r, cz - r, // 1
        cx + r, cy + r, cz - r, // 2
        cx - r, cy + r, cz - r, // 3
        cx - r, cy - r, cz + r, // 4
        cx + r, cy - r, cz + r, // 5
        cx + r, cy + r, cz + r, // 6
        cx - r, cy + r, cz + r  // 7
    };
    return vertices;
}
std::vector<unsigned int> generateCubeIndices() {
    return {
        // Back face
        0, 1, 2,
        2, 3, 0,
        // Front face
        4, 5, 6,
        6, 7, 4,
        // Left face
        4, 7, 3,
        3, 0, 4,
        // Right face
        1, 5, 6,
        6, 2, 1,
        // Bottom face
        4, 5, 1,
        1, 0, 4,
        // Top face
        3, 2, 6,
        6, 7, 3
    };
}

std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    return vertices;
}

std::vector<unsigned int> generateSphereIndices(int sectorCount, int stackCount) {
    std::vector<unsigned int> indices;
    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            unsigned int first = (i * (sectorCount + 1)) + j;
            unsigned int second = first + sectorCount + 1;
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    return indices;
}
