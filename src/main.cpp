#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
std::atomic<float> xOffset(0.0f);
std::atomic<float> yOffset(0.0f);
// Vertex shader source

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec2 uOffset;
void main() {
    gl_Position = vec4(aPos.x + uOffset.x, aPos.y + uOffset.y, aPos.z, 1.0);
}
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";
struct VertexLabel {
    float x1, y1, z1;
    float x2, y2, z2;
    float x3, y3, z3;
};
struct Point {
    float x, y;
};

class Objects {
    public:
        Objects(std::vector<float> init_vertices, Point init_acceleration, Point init_velocity);
        std::vector<float> vertices;
        Point acceleration;
        Point velocity;
        unsigned int VAO, VBO;
        unsigned int shaderProgram;
        void initialize();
        void render();
        void cleanup();
        
};

Objects::Objects(std::vector<float> init_vertices, Point init_acceleration, Point init_velocity) {
    vertices = init_vertices;
    acceleration = init_acceleration;
    velocity = init_velocity;
}
void Objects::initialize() {
    // Vertex Array Object, Vertex Buffer Object
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
void Objects::render() {
        float x_offset = xOffset.load();  // Atomic read
        float y_offset = yOffset.load();  // Atomic read
        int offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glUniform2f(offsetLoc, x_offset, y_offset);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Objects::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        xOffset = xOffset - .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        xOffset = xOffset + .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        yOffset = yOffset + 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        yOffset = yOffset - 0.01f;
    }
}
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSwapInterval(1);  // <-- NEW: Turn on V-Sync!

    // Vertex data
    Objects triangle({0.0f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f}, {0, 0}, {0, 0});
    triangle.initialize();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        triangle.render();     
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    triangle.cleanup();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
