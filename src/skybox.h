#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include "render/Shader.h"
class Skybox {
public:
    // Constructor
    Skybox(const std::string& singleImagePath);

    // Destructor
    ~Skybox();
    
    // Initialize the skybox
    void initialize();

    // Render the skybox
    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
    unsigned int VAO, VBO, cubemapTexture;
    Shader skyprogram;
    //unsigned int shaderProgram;
    std::string singleImagePath; // Store the single image path

    // Load cubemap texture from a single image
    unsigned int loadCubemap(const std::string& singleImagePath);
};


#endif // SKYBOX_H
