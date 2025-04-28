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
#include "objects.hpp"
#include "shaders.hpp"
#include "camera.hpp"
#include "structs.hpp"

void processInput(GLFWwindow* window) {
    float cameraRotateSpeed = 1.0f; // degrees per frame
    float cameraSpeed = 0.5f; // degrees per frame
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yaw -= cameraRotateSpeed;
        updateCameraVectors();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yaw += cameraRotateSpeed;
        updateCameraVectors();
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pitch += cameraRotateSpeed;
        if (pitch > 89.0f) pitch = 89.0f;
        updateCameraVectors();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pitch -= cameraRotateSpeed;
        if (pitch < -89.0f) pitch = -89.0f;
        updateCameraVectors();
    }
}



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1700, 1100, "SolarSim", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSwapInterval(1); // Enable V-Sync
    std::vector<float> vertices = generateSphereVertices(0.5f, 16, 32);
    std::vector<unsigned int> indices = generateSphereIndices(16, 32);
    Point acceleration = {0, -0.005, 0};
    Point velocity = {0, 0, 0};
    std::vector<Objects> spheres;
    for (int i = 0; i < 10000; ++i) {
        Point position = {(i % 10) * 2.0f, 50.0f, (i / 10) * 2.0f};
        spheres.emplace_back(vertices, indices, acceleration, velocity, position);
    }

    compileShaders();
    for (auto& sphere : spheres) {
        sphere.initialize();
    }
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        setCamera();
        for (auto& sphere : spheres) {
            sphere.update();
            sphere.render();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto& sphere : spheres) {
        sphere.cleanup();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
