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

void processInput(GLFWwindow* window, Objects& obj) {
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

    std::vector<float> vertices = generateCubeVertices(0.f, 0.f, 0.f, 0.5f);
    std::vector<unsigned int> indices = generateCubeIndices();

    Objects cube(vertices, indices, {0, 0, 0}, {0, 0, 0}, {0, 0, 0});

    compileShaders();
    cube.initialize();
    glEnable(GL_DEPTH_TEST);
    cube.acceleration = {0.0005f, 0.f, 0.f};
    while (!glfwWindowShouldClose(window)) {
        processInput(window, cube);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        setCamera();
        cube.update();
        cube.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cube.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
