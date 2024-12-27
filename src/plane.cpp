#include "plane.h"
#include "render/shader.h" // Ensure this is included for LoadShadersFromFile
#include <iostream>

Plane::Plane() : vao(0), vbo(0), ebo(0), shaderProgram(0) {}

Plane::~Plane() {
    cleanup();
}

void Plane::initialize() {
    float size = 100.0f;
    float vertices[] = {
        // Positions        // UVs
        -size, 0.0f, -size,  0.0f, 0.0f,
         size, 0.0f, -size,  1.0f, 0.0f,
         size, 0.0f,  size,  1.0f, 1.0f,
        -size, 0.0f,  size,  0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // UV
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    shaderProgram = LoadShadersFromFile("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.frag");
    if (shaderProgram == 0)
    { 
        std::cerr << "Failed to load shaders for PLANE." << std::endl;
    }
}

void Plane::render(const glm::mat4& vp, const glm::mat4& modelMatrix) {
    glUseProgram(shaderProgram);

    glm::mat4 mvp = vp * modelMatrix;
    GLuint mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

    //GLuint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    //glUniform1i(useTextureLoc, 0); // Disable texture

    GLuint solidColorLoc = glGetUniformLocation(shaderProgram, "solidColor");
    glUniform4f(solidColorLoc, 0.0f, 1.0f, 0.0f, 1.0f); // Solid green

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Plane::cleanup() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
}
