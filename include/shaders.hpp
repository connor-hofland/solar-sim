#ifndef SHADERS_HPP
#define SHADERS_HPP
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;
extern unsigned int shaderProgram;
void compileShaders();
#endif
