#ifndef PLANE_H
#define PLANE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <render/Shader.h>
class Plane {
public:
    Plane();
    ~Plane();

    void initialize(Shader shader, float size);
    //void render(const glm::mat4& vp, const glm::mat4& modelMatrix);
    void render(Shader shader, const glm::mat4 vp,
        glm::mat4 modelMatrix,
        const glm::vec3 viewPosition,
        bool shad, glm::vec3 lightPos, const glm::vec3 position);
    void cleanup();

    void Plane::shadowRender(Shader simpleDepthShader, GLuint depthMapFBO);
private:
    unsigned int vao, vbo, ebo;
    unsigned int textureID;
    Shader shader;
    //Shader shadowdepthloadprogram;

    // GLuint shaderProgram; // Use GLuint to hold the shader program ID
};

#endif // PLANE_H
