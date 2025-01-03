#include "plane.h"
#include "render/Shader.h" // Ensure this is included for LoadShadersFromFile
#include <iostream>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <render/Shader.h>


//Shader shadowloadprogram = Shader(
//    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/shadow_mapping.vs",
//    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/shadow_mapping.fs"
//);
//Shader shadowdepthloadprogram = Shader(
//    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/shadow_mapping_depth.vert",
//    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/shadow_mapping_depth.frag"
//);

//planeprogram
//shadowloadprogram
//shadowdepthloadprogram

Plane::Plane() : vao(0), vbo(0), ebo(0), textureID(0), instanceVBO(0), shader(
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.frag"
    ) {}

Plane::~Plane() {
    cleanup();
}


//void Plane::initialize(Shader shader, float size) {
//    float vertices[] = {
//        //       Position        |       Normal        |   UV
//        //   x       y      z      nx   ny   nz         u    v
//        -size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
//         size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
//         size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
//        -size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f
//    };
//
//    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
//
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &vbo);
//    glGenBuffers(1, &ebo);
//
//    glBindVertexArray(vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);               // position
//    glEnableVertexAttribArray(0);
//
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normal
//    glEnableVertexAttribArray(1);
//
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
//    glEnableVertexAttribArray(2);
//
//    glBindVertexArray(0);
//
//  /*  Shader planeprogram = Shader("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.vert",
//        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/plane.frag");*/
//    if (shader.ID == 0)
//    { 
//        std::cerr << "Failed to load shaders for PLANE." << std::endl;
//    }
//
//    // Load GLTF file
//    tinygltf::Model model;
//    tinygltf::TinyGLTF loader;
//    std::string err, warn;
//
//    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, 
//        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/rocky_terrain_02_1k/rocky_terrain_02_1k.gltf");
//    if (!warn.empty()) {
//        std::cerr << "Warning: " << warn << std::endl;
//    }
//    if (!err.empty()) {
//        std::cerr << "Error: " << err << std::endl;
//    }
//    if (!ret) {
//        std::cerr << "Failed to load GLTF file!" << std::endl;
//        return;
//    }
//
//    // Extract texture from GLTF file
//    const tinygltf::Image& image = model.images[0]; // Assuming the first image is your texture
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(
//        GL_TEXTURE_2D,
//        0,
//        GL_RGBA,
//        image.width,
//        image.height,
//        0,
//        GL_RGBA,
//        GL_UNSIGNED_BYTE,
//        image.image.data()
//    );
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    // Set texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    glBindTexture(GL_TEXTURE_2D, 0);
//}


//void Plane::initialize(Shader shader, float size) {
//    float textureRepeat = size; // Controls how many times the texture repeats across the plane
//    if (size > 100) {
//        textureRepeat = 120;
//    }
//    float vertices[] = {
//        //       Position        |       Normal        |   UV
//        //   x       y      z      nx   ny   nz         u    v
//        -size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
//         size,  0.0f, -size,     0.0f, 1.0f, 0.0f,     textureRepeat, 0.0f,
//         size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     textureRepeat, textureRepeat,
//        -size,  0.0f,  size,     0.0f, 1.0f, 0.0f,     0.0f, textureRepeat
//    };
//
//    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
//
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &instanceVBO);
//    glGenBuffers(1, &ebo);
//
//    glBindVertexArray(vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);               // position
//    glEnableVertexAttribArray(0);
//
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normal
//    glEnableVertexAttribArray(1);
//
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
//    glEnableVertexAttribArray(2);
//
//    glBindVertexArray(0);
//
//    if (shader.ID == 0) {
//        std::cerr << "Failed to load shaders for PLANE." << std::endl;
//    }
//
//    // Load GLTF file (if applicable)
//    tinygltf::Model model;
//    tinygltf::TinyGLTF loader;
//    std::string err, warn;
//
//    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn,
//        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/coast_sand_rocks_02_2k/coast_sand_rocks_02_2k.gltf");
//    if (!warn.empty()) {
//        std::cerr << "Warning: " << warn << std::endl;
//    }
//    if (!err.empty()) {
//        std::cerr << "Error: " << err << std::endl;
//    }
//    if (!ret) {
//        std::cerr << "Failed to load GLTF file!" << std::endl;
//        return;
//    }
//
//    // Extract texture from GLTF file
//    const tinygltf::Image& image = model.images[1]; // Assuming the first image is your texture
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(
//        GL_TEXTURE_2D,
//        0,
//        GL_RGBA,
//        image.width,
//        image.height,
//        0,
//        GL_RGBA,
//        GL_UNSIGNED_BYTE,
//        image.image.data()
//    );
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    // Set texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//
//
//    std::vector<glm::mat4> planeInstanceMatrices;
//
//    glm::mat4 instance = glm::mat4(1.0f);
//    instance = glm::scale(instance, glm::vec3(2.0f));
//    planeInstanceMatrices.push_back(instance);
//
//
//    glm::mat4 modelMatrix = glm::mat4(1.0f);
//    glGenBuffers(1, &instanceVBO); // Generate the instance VBO
//    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &planeInstanceMatrices[0], GL_STATIC_DRAW);
//
//    // Allocate space for one model matrix (4x4 floats)
//
//    std::size_t vec4Size = sizeof(glm::vec4);
//    // Configure instance attribute pointers (4 vec4s for a mat4)
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
//    glEnableVertexAttribArray(4);
//    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
//    glEnableVertexAttribArray(5);
//    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
//    glEnableVertexAttribArray(6);
//    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
//
//    glVertexAttribDivisor(3, 1);
//    glVertexAttribDivisor(4, 1);
//    glVertexAttribDivisor(5, 1);
//    glVertexAttribDivisor(6, 1);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//}

void Plane::initialize(Shader shader, float size) {
    float textureRepeat = (size > 100) ? 120 : size; // Control texture tiling
    float vertices[] = {
        //    Positions         | Normals      |  UV
        -size,  0.0f, -size,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         size,  0.0f, -size,     0.0f, 1.0f, 0.0f,   textureRepeat, 0.0f,
         size,  0.0f,  size,     0.0f, 1.0f, 0.0f,   textureRepeat, textureRepeat,
        -size,  0.0f,  size,     0.0f, 1.0f, 0.0f,   0.0f, textureRepeat
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    // Generate buffers and vertex array object (VAO)
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &instanceVBO);

    // Bind VAO and set up vertex attributes
    glBindVertexArray(vao);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Bind and upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Prepare instance data
    glm::mat4 instanceMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)); // Example scaling
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(instanceMatrix), GL_STATIC_DRAW);

    // Set up instance attributes (mat4 requires 4 vec4 attributes)
    std::size_t vec4Size = sizeof(glm::vec4);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0); // Unbind VAO

    // Load texture from GLTF
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn,
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/rocky_terrain_02_1k/rocky_terrain_02_1k.gltf"); // Update with the correct path
    if (!warn.empty()) std::cerr << "Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Error: " << err << std::endl;
    if (!ret) {
        std::cerr << "Failed to load GLTF file!" << std::endl;
        return;
    }

    // Extract and bind texture
    const tinygltf::Image& image = model.images[2]; // Ensure correct index
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

void Plane::render(Shader shader, const glm::mat4 vp,
    glm::mat4 modelMatrix,
    const glm::vec3 viewPosition,
    bool shad, glm::vec3 lightPos, const glm::vec3 position)
{
    // Use our "final pass" shadow_mapping shader program
    glUseProgram(shader.ID);

    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    // ------------------------------------------------------------
    // 1) Resolve the "projection" and "view" from `vp`
    //    (We do a trick: 'projection' = vp, 'view' = identity)
    // ------------------------------------------------------------
    //glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 instanceModelMatrix = glm::translate(glm::mat4(1.0f), position);
    //GLuint projLoc = glGetUniformLocation(planeprogram.ID, "projection");
    //GLuint viewLoc = glGetUniformLocation(planeprogram.ID, "view");
    //GLuint modelLoc = glGetUniformLocation(planeprogram.ID, "model");
    // 
    // Update instance model matrix in instanceVBO
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(instanceModelMatrix));
    
    //// We set `projection = vp` so the final multiply in the vertex shader
    //// becomes: projection * view * model = (vp) * (identity) * model.
    ////glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(vp));
    ////glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(identity));
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    //shader.setMat4("model", modelMatrix);
    // ------------------------------------------------------------
    // 2) Light-space matrix (for shadow-casting)
    //    If you want real shadows, you must also set this uniform
    //    from outside, or pass it in here. For demonstration, let's
    //    just set it to identity if not available:
    // ------------------------------------------------------------
    //GLuint lsmLoc = glGetUniformLocation(planeprogram.ID, "lightSpaceMatrix");
    //if (lsmLoc != -1) {
    //    glm::mat4 identityLS = glm::mat4(1.0f); // Placeholder if you don't pass it in
    //    glUniformMatrix4fv(lsmLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    //}

    // ------------------------------------------------------------
    // 3) Light & camera uniforms
    // ------------------------------------------------------------
    //GLuint lightPosLoc = glGetUniformLocation(planeprogram.ID, "lightPos");
    //if (lightPosLoc != -1)
        //glUniform3f(lightPosLoc, -2.0f, 4.0f, -1.0f);  // or any real light pos
        //glUniform3f(lightPosLoc, 10.0f, 10.0f, 10.0f);  // or any real light pos

    //if (lightPosLoc != -1)
    //    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    // ------------------------------------------------------------
    // 4) Diffuse texture (plane's color)
    // ------------------------------------------------------------
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), 1);
    }
    else {
        glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), 0);
    }
    //GLint diffTexLoc = glGetUniformLocation(planeprogram.ID, "diffuseTexture");
    //if (diffTexLoc != -1)
    //    glUniform1i(diffTexLoc, 0); // texture unit 0

    // ------------------------------------------------------------
    // 5) Shadow map texture
    //    Usually bound to texture unit 1 (the code in main.cpp does this)
    //    We'll just assume it is already bound externally, or we can do:
    //        glActiveTexture(GL_TEXTURE1);
    //        glBindTexture(GL_TEXTURE_2D, depthMap); // You need to pass that in
    //        glUniform1i(glGetUniformLocation(planeprogram.ID, "shadowMap"), 1);
    //    But often we do that from main.cpp.
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // 6) Finally draw the plane
    // ------------------------------------------------------------
    glBindVertexArray(vao);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
    glBindVertexArray(0);

    glUseProgram(0);
}








//void Plane::shadowRender(const glm::mat4& lightSpaceMatrix, GLuint depthMapFBO) {
//
//    //glUseProgram(shadowdepthloadprogram.ID);
//    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//    glClear(GL_DEPTH_BUFFER_BIT);
//
//    glUseProgram(shadowdepthloadprogram.ID);
//
//    GLuint lightSpaceMatrixLocation = glGetUniformLocation(shadowdepthloadprogram.ID, "lightSpaceMatrix");
//    if (lightSpaceMatrixLocation != -1) {
//        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//    }
//
//
//    // (Optional) If your "shadow_mapping_depth.vert" expects a "model" matrix uniform, do it here:
//    //   GLuint modelLoc = glGetUniformLocation(shadowdepthloadprogram.ID, "model");
//    //   if (modelLoc != -1) {
//    //       glm::mat4 planeModel = glm::mat4(1.0f); // or your actual model transform
//    //       glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planeModel));
//    //   }
//
//    glBindVertexArray(vao);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    glUseProgram(0);
//}


void Plane::shadowRender(Shader simpleDepthShader, GLuint depthMapFBO)
{
     //1) Bind the depth FBO & clear
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // 2) Use the shadow depth shader
    glUseProgram(simpleDepthShader.ID);

    glm::mat4 planeModel = glm::mat4(1.0f);
    simpleDepthShader.setMat4("model", planeModel);
    // 4) Set "model" for the planeee
    //GLint modelLoc = glGetUniformLocation(shadowdepthloadprogram.ID, "model");
    //if (modelLoc != -1)
    //{
    //    // If your plane is at y=0 with no extra transform:
    //    // Or if you want a shift, scale, rotation, etc., do it here
    //    // planeModel = glm::translate(planeModel, glm::vec3(...));
    //    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planeModel));
    //}
    //glm::mat4 identity = glm::mat4(1.0f);

    // We set `projection = vp` so the final multiply in the vertex shader
    // becomes: projection * view * model = (vp) * (identity) * model.
    // 5) Draw
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 6) Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}



void Plane::cleanup() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shader.ID);
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