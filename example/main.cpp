#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_utils.h"

// ������ü�� ���� ������ (X, Y, Z ��ǥ)
GLfloat vertices[] = {
        // ������ü�� ���� ������ (X, Y, Z ��ǥ)�� �麰 ����
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // ���� �Ʒ� �޸� ������ (0)
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // ������ �Ʒ� �޸� ������ (1)
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // ������ �� �޸� ������ (2)
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // ���� �� �޸� ������ (3)
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, // ���� �Ʒ� �ո� ������ (4)
        0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // ������ �Ʒ� �ո� ������ (5)
        0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  // ������ �� �ո� ������ (6)
        -0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 0.5f   // ���� �� �ո� ������ (7)
};

// ����� �ε��� ������: �޸��� ������ ������ ����
GLuint indices[] = {
        1, 0, 3,  // �޸� �ﰢ�� 1 (���� ����)
        1, 3, 2,  // �޸� �ﰢ�� 2 (���� ����)
        4, 5, 6,  // �ո� �ﰢ�� 1
        4, 6, 7,  // �ո� �ﰢ�� 2
        0, 1, 5,  // ���� �� �ﰢ�� 1 (���� ����)
        0, 5, 4,  // ���� �� �ﰢ�� 2 (���� ����)
        5, 1, 2,  // ������ �� �ﰢ�� 1 (���� ����)
        5, 2, 6,  // ������ �� �ﰢ�� 2 (���� ����)
        0, 4, 7,  // �Ʒ� �� �ﰢ�� 1 (���� ����)
        0, 7, 3,  // �Ʒ� �� �ﰢ�� 2 (���� ����)
        2, 3, 7,  // �� �� �ﰢ�� 1
        2, 7, 6   // �� �� �ﰢ�� 2
};

const std::string BASE_PATH = "/Users/dgsw8th71/Desktop/develop/MyGameEngine/example";
const std::string vertexShaderPath = BASE_PATH + "/vertex_shader.glsl";
const std::string fragmentShaderPath = BASE_PATH + "/fragment_shader.glsl";

bool initSDL(SDL_Window** window, SDL_GLContext* context) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed" << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    *window = SDL_CreateWindow(
            "Modern OpenGL Rendering",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1200,
            800,
            SDL_WINDOW_OPENGL
    );

    if (!*window) {
        std::cerr << "Window creation failed" << std::endl;
        return false;
    }

    *context = SDL_GL_CreateContext(*window);
    if (*context == nullptr) {
        std::cerr << "OpenGL context creation failed" << std::endl;
        return false;
    }

    return true;
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open GLSL file '" << filePath << "': " << strerror(errno) << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    if (!initSDL(&window, &context)) {
        std::cerr << "SDL initialization failed" << std::endl;
        return -1;
    }

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(glewInitResult) << std::endl;
        return -1;
    }

    std::string vertexShaderSource = readFile(vertexShaderPath);
    std::string fragmentShaderSource = readFile(fragmentShaderPath);
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        std::cerr << "Shader file error" << std::endl;
        return -1;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
        return -1;
    }

    GLuint VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        GLfloat angle = SDL_GetTicks() / 1000.0f * 45.0f;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(rotationMatrix));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
