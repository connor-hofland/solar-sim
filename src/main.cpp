#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <atomic>
#include <vector>

// Global shader program
unsigned int shaderProgram;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw   = -90.0f; // starting pointing toward negative z
float pitch = 0.0f;   // starting level

enum ObjectType {
    TRIANGLE,
    SPHERE
};

struct Point {
    float x, y, z;
};

class Objects {
public:
    Objects(std::vector<float> init_vertices, std::vector<unsigned int> init_indices, Point init_acceleration, Point init_velocity, Point init_offset, const ObjectType init_type);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    Point acceleration;
    Point velocity;
    Point offset;
    const ObjectType type;

    unsigned int VAO, VBO, EBO;

    void initialize();
    void render();
    void update();
    void cleanup();
};

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform vec3 uOffset;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(aPos + uOffset, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    void main() {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
)";

Objects::Objects(std::vector<float> init_vertices, std::vector<unsigned int> init_indices, Point init_acceleration, Point init_velocity, Point init_offset, const ObjectType init_type)
    : vertices(init_vertices), indices(init_indices), acceleration(init_acceleration), velocity(init_velocity), offset(init_offset), type(init_type) {}

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
    if (type == SPHERE) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
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
void updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

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

void compileShaders() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::VERTEX_SHADER\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::FRAGMENT_SHADER\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setCamera() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                            1700.0f / 1100.0f, 
                                            0.1f, 100.0f);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1700, 1100, "Sphere", nullptr, nullptr);
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

    std::vector<float> vertices = generateSphereVertices(0.5f, 36, 18);
    std::vector<unsigned int> indices = generateSphereIndices(36, 18);

    Objects sphere(vertices, indices, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, SPHERE);

    compileShaders();
    sphere.initialize();
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, sphere);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        setCamera();
        sphere.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    sphere.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

