#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

const char* vertexSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";

const char* fragmentSrc = R"(
#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0, 0.5, 0.2, 1.0); }
)";

int main() {
    int n;
    std::cout << "Этаж саны / Количество этажей: ";
    std::cin >> n;
    if (n < 1) n = 1;

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 800, "Pyramid", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) return -1;

    // --- вершины пирамиды ---
    std::vector<float> vertices;
    const float W = 1.8f;
    const float a = W / n;
    const float h = a * std::sqrt(3.0f) / 2.0f;
    const float yTop = (n * h) / 2.0f;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            float cx = (j - i / 2.0f) * a;
            float yA = yTop - i * h;
            float yB = yA - h;
            vertices.insert(vertices.end(), {
                cx,       yA, 0.0f,
                cx - a/2, yB, 0.0f,
                cx + a/2, yB, 0.0f
            });
        }
    }
    const int vertexCount = (int)(vertices.size() / 3);

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // --- шейдеры ---
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);
    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    glDeleteShader(vs);
    glDeleteShader(fs);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}