#include "skybox.h"
#include "render/Shader.h" // For LoadShadersFromFile
#include <stb_image.h> // For image loading
#include <iostream>

// Skybox vertices
static const float skyboxVertices[] = {
    // Positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox(const std::string& singleImagePath)
    : singleImagePath(singleImagePath), VAO(0), VBO(0), cubemapTexture(0), skyprogram("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/skybox.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/skybox.frag") {
}

// Destructor
Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &cubemapTexture);
}



void Skybox::initialize() {
    // Generate VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Load cubemap texture from single image
    cubemapTexture = loadCubemap(singleImagePath);

    // Load skybox shaders
  /*  program= Shader("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/skybox.vert", 
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/skybox.frag");*/
}


// Load cubemap texture
//unsigned int Skybox::loadCubemap(const std::vector<std::string>& faces) {
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//    int width, height, nrChannels;
//    for (unsigned int i = 0; i < faces.size(); i++) {
//        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//        if (data) {
//            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//            stbi_image_free(data);
//        }
//        else {
//            std::cerr << "Failed to load cubemap texture at " << faces[i] << std::endl;
//            stbi_image_free(data);
//        }
//    }
//
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//    return textureID;
//}


unsigned int Skybox::loadCubemap(const std::string& singleImagePath) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Load the single image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(singleImagePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load cubemap image: " << singleImagePath << std::endl;
        return 0;
    }

    // Calculate the size of each face (assuming the input image is 3x2 grid)
    int faceWidth = width / 4;
    int faceHeight = height / 3;

    // Define offsets for each cubemap face
    struct Offset { int x, y; };
    Offset faceOffsets[6] = {
        {2 * faceWidth, 1 * faceHeight}, // posX
        {0 * faceWidth, 1 * faceHeight}, // negX
        {1 * faceWidth, 0 * faceHeight}, // posY
        {1 * faceWidth, 2 * faceHeight}, // negY
        {1 * faceWidth, 1 * faceHeight}, // posZ
        {3 * faceWidth, 1 * faceHeight}  // negZ
    };

    // Extract each face and upload to OpenGL
    for (int i = 0; i < 6; ++i) {
        int xOffset = faceOffsets[i].x;
        int yOffset = faceOffsets[i].y;

        // Allocate memory for a single face
        unsigned char* faceData = new unsigned char[faceWidth * faceHeight * nrChannels];
        for (int row = 0; row < faceHeight; ++row) {
            int sourceY = yOffset + row;
            memcpy(
                faceData + (row * faceWidth * nrChannels),
                data + ((sourceY * width + xOffset) * nrChannels),
                faceWidth * nrChannels
            );
        }

        // Upload the face to the cubemap
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB,
            faceWidth,
            faceHeight,
            0,
            nrChannels == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            faceData
        );

        delete[] faceData;
    }

    // Free the original image
    stbi_image_free(data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


// Render the skybox
void Skybox::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glDepthFunc(GL_LEQUAL); // Make sure skybox is rendered in the background
    glUseProgram(skyprogram.ID);

    // Remove translation from the view matrix
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));
    GLuint viewLoc = glGetUniformLocation(skyprogram.ID, "view");
    GLuint projectionLoc = glGetUniformLocation(skyprogram.ID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewNoTranslation[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // Reset depth function
}
