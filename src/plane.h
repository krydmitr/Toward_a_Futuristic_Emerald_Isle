#ifndef PLANE_H
#define PLANE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Plane {
public:
    Plane();
    ~Plane();

    void initialize();
    void render(const glm::mat4& vp, const glm::mat4& modelMatrix);
    void cleanup();

private:
    unsigned int vao, vbo, ebo;
    GLuint shaderProgram; // Use GLuint to hold the shader program ID
};

#endif // PLANE_H
