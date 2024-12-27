#include "plane.h"
#include "render/shader.h" // Ensure this is included for LoadShadersFromFile
#include <iostream>
#include <stb_image.h>
#include <tiny_gltf.h>

Plane::Plane() : vao(0), vbo(0), ebo(0), shaderProgram(0), textureID(0){}

Plane::~Plane() {
    cleanup();
}

void Plane::initialize() {
    float size = 100.0f;
    float vertices[] = {
        //       Position        |       Normal        |   UV
        //   x       y      z      nx   ny   nz         u    v
        -size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
         size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
         size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        -size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);               // position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    shaderProgram = LoadShadersFromFile("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.frag");
    if (shaderProgram == 0)
    { 
        std::cerr << "Failed to load shaders for PLANE." << std::endl;
    }

    // Load GLTF file
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, 
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/rocky_terrain_02_1k/rocky_terrain_02_1k.gltf");
    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!ret) {
        std::cerr << "Failed to load GLTF file!" << std::endl;
        return;
    }

    // Extract texture from GLTF file
    const tinygltf::Image& image = model.images[0]; // Assuming the first image is your texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image.width,
        image.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image.image.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

// void Plane::render(const glm::mat4& vp, const glm::mat4& modelMatrix) {
//     glUseProgram(shaderProgram);
// 
//     glm::mat4 mvp = vp * modelMatrix;
//     GLuint mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
//     glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);
// 
//     //GLuint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
//     //glUniform1i(useTextureLoc, 0); // Disable texture
// 
//     GLuint solidColorLoc = glGetUniformLocation(shaderProgram, "solidColor");
//     glUniform4f(solidColorLoc, 0.0f, 1.0f, 0.0f, 1.0f); // Solid green
// 
//     glBindVertexArray(vao);
//     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//     glBindVertexArray(0);
// 
//     glUseProgram(0);
// }



void Plane::render(const glm::mat4& vp, const glm::mat4& modelMatrix, const glm::vec3& viewPosition) {
    glUseProgram(shaderProgram);

    // MVP Matrix
    glm::mat4 mvp = vp * modelMatrix;
    GLuint mvpLocation = glGetUniformLocation(shaderProgram, "MVP");
    if (mvpLocation == -1) {
        std::cerr << "Failed to find MVP uniform location in plane shader!" << std::endl;
        return;
    }
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);


    GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    if (modelMatrixLocation != -1) {
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    }
    else {
        std::cerr << "Failed to find modelMatrix uniform location in plane shader!" << std::endl;
    }




    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    GLuint normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
    if (normalMatrixLoc != -1) {
        glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
    }


    // Set Light Position and Intensity
    glm::vec3 lightPosition(10.0f, 10.0f, 10.0f); // Example light position
    //glm::vec3 lightIntensity(1.0f, 1.0f, 1.0f);  // Example light intensity
    glm::vec3 lightIntensity(100.0f, 100.0f, 100.0f); // Brighter light


    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPosition");
    GLuint lightIntLoc = glGetUniformLocation(shaderProgram, "lightIntensity");

    if (lightPosLoc == -1 || lightIntLoc == -1) {
        std::cerr << "Failed to get uniform locations for light!" << std::endl;
    }

    if (lightPosLoc != -1) glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    if (lightIntLoc != -1) glUniform3fv(lightIntLoc, 1, &lightIntensity[0]);

    // Pass the camera position (view position) to the shader
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPosition");
    if (viewPosLoc == -1) {
        std::cerr << "Failed to get uniform location for viewPosition!" << std::endl;
    }
    if (viewPosLoc != -1) glUniform3fv(viewPosLoc, 1, &viewPosition[0]);

    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // Texture unit 0
    if (textureID == 0) {
        std::cerr << "Texture ID is invalid. Texture not loaded!" << std::endl;
    }
    // Draw the plane
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);


    //std::cerr << "Uniform locations - MVP: " << mvpLocation
    //    << ", ModelMatrix: " << modelMatrixLocation
    //    << ", LightPosition: " << lightPosLoc
    //    << ", LightIntensity: " << lightIntLoc
    //    << ", ViewPosition: " << viewPosLoc << std::endl;



}








void Plane::cleanup() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load image using stb_image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}