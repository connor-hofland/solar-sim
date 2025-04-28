#ifndef OBJECTS_HPP
#define OBJECTS_HPP
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include "structs.hpp"
class Objects {
public:
    Objects(std::vector<float> init_vertices, std::vector<unsigned int> init_indices, Point init_acceleration, Point init_velocity, Point init_offset);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    Point acceleration;
    Point velocity;
    Point offset;

    unsigned int VAO, VBO, EBO;

    void initialize();
    void render();
    void update();
    void cleanup();
};
std::vector<float> generateCubeVertices(float cx, float cy, float cz, float r);
std::vector<unsigned int> generateCubeIndices();
std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount);
std::vector<unsigned int> generateSphereIndices(int sectorCount, int stackCount);
#endif
