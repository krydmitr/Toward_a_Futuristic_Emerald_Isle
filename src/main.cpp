#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <iomanip>
#include <math.h>
#include <sstream>

// Your includes
#include "skybox.h"
#include "plane.h"
#include "loader.h"  // MyBot
#include "render/Shader.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// GLOBALS
static GLFWwindow* window;
static int windowWidth = 1024;
static int windowHeight = 768;

// Camera
static float cameraSpeed = 1;
static glm::vec3 eye_center = glm::vec3(4.0f, 0.0f, -5.0f);
static glm::vec3 lookat = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

static float FoV = 45.0f;
static float zNear = 1.0f;
static float zFar = 2000.0f;

// For shadow pass
float lightVertices[] = {
    // positions          // normals           // texture coords
    // Front face
    -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    // Back face
    -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
     0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

    // Left face
    -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.1f,  0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.1f, -0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

    // Right face
     0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.1f,  0.1f, -0.1f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -0.1f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

     // Bottom face
     -0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
      0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     -0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     -0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

     // Top face
     -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
      0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     -0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
};
float quadVertices[] = {
    // Positions   // Texture Coords
    -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
    -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
     1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

    -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
     1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
     1.0f,  1.0f,  1.0f, 1.0f  // Top-right
};

void renderRedCubeAtPosition(const Shader& shader, unsigned int VAO, unsigned int texture, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& position) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position); // Move cube to specified position
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f)); // Scale cube for visibility

    shader.setMat4("model", modelMatrix);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("diffuseTexture", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

struct Chunk {
    glm::vec3 position;
    Plane ground;

    MyBot buildings;
    MyBot aircraft;
    MyBot car;



    void chunkinitialise(Shader& shader, Shader& simpleDepthShader) {
        ground.initialize(shader);

        //buildings.renderInstancing;
        aircraft.initialize(shader, simpleDepthShader);
        //car.initialize(shader, simpleDepthShader);
    }



    //bot.render(shader, vp, projectionMatrix, viewMatrix, true, lightPos);
    //plane.render(shader, vp, modelMatrix, eye_center, true, lightPos);
    void chunkrender(Shader& shader, glm::mat4 vp, glm::mat4 projectionMatrix, glm::mat4 viewMatrix,
        bool shadows, glm::vec3 lightPoschunk, glm::mat4 modelMatrix, glm::vec3 renderposition) {
        glm::vec3 zero(0.0f, 0.0f, 0.0f);
        ground.render(shader, vp, modelMatrix, eye_center, true, lightPoschunk, position);

        
        
        
        
        // SETUP MODELMATRIX FOR EACH PIECE THAT YOU WANT IN THE CHUNK 

        
        //glm::mat4 modelMatrix = glm::mat4(1.0f);
        //modelMatrix = glm::translate(modelMatrix, position); // Apply position
        //modelMatrix *= glm::mat4_cast(rotation);            // Apply rotation
        //modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f)); // Optional scaling




        //buildings.renderInstancing;
        aircraft.render(shader, vp, projectionMatrix, viewMatrix, true, lightPoschunk, position, modelMatrix);
        //car.render(shader, vp, projectionMatrix, viewMatrix, true, lightPoschunk);    
    }

    void chunkshadowrender(Shader& simpleDepthShader, unsigned int depthMapFBO) {
        //ground.render();

        //buildings.renderInstancing;
        aircraft.shadowRender(simpleDepthShader, depthMapFBO, position);
        //car.shadowRender(simpleDepthShader, depthMapFBO);
    }
};



const int GRID_SIZE = 4;       // Number of chunks along one dimension
const float CHUNK_SPACING = 2.1f; // Distance between chunks
std::vector<Chunk> generateChunks(Shader shader, Shader simpleDepthShader) {
    std::vector<Chunk> chunks;

    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int z = 0; z < GRID_SIZE; ++z) {
            Chunk chunk;
            chunk.position = glm::vec3(x * CHUNK_SPACING, 0.0f, z * CHUNK_SPACING);
            chunks.push_back(chunk);
        }
    }
    return chunks;
}


void chunksinitialise(std::vector<Chunk>& chunks, Shader& shader, Shader& simpleDepthShader) {
    //ground.render();

    for (int i = 0; i < chunks.size(); i++) {
        chunks[i].chunkinitialise(shader, simpleDepthShader);
    }
}



void chunksshadowrender(std::vector<Chunk>& chunks, Shader& simpleDepthShader, unsigned int depthMapFBO) {

    for (int i = 0; i < chunks.size(); i++) {
        chunks[i].chunkshadowrender(simpleDepthShader, depthMapFBO);
    }
}



void chunksrender(std::vector<Chunk>& chunks, Shader& shader, glm::mat4 vp, glm::mat4 projectionMatrix, glm::mat4 viewMatrix,
        bool shadows, glm::vec3 lightPoschunk, glm::mat4 modelMatrix, glm::vec3 renderposition) {
 
     
    for (int i = 0; i < chunks.size(); i++) {
        chunks[i].chunkrender(shader, vp, projectionMatrix, viewMatrix,
            shadows, lightPoschunk, modelMatrix, chunks[i].position);
    }
}




//std::vector<Chunk> chunks = generateChunks(shader, simpleDepthShader);
//
//chunksinitialise(chunks, shader, simpleDepthShader);
//
//chunksshadowrender(chunks,  simpleDepthShader,  depthMapFBO);
//
//chunksrender( chunks,  shader,  vp,  projectionMatrix,  viewMatrix,  shadows,  lightPoschunk,  modelMatrix,  renderposition);


static int currentDepthFace = 0; // 0 to 5
int main()
{

    // --------------------------------------------------
    // 1) Initialize GLFW and Create Window
    // --------------------------------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

    // --------------------------------------------------
    // 2) Load GL via glad
    // --------------------------------------------------
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cerr << "Failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // --------------------------------------------------
    // 3) Create Shaders for depth pass & final pass
    //    (They are also used internally by your classes)
    // --------------------------------------------------




    Shader shader(
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.frag"
    );
    Shader simpleDepthShader(
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.frag",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.gs"
    );

    // --------------------------------------------------
    // 4) Create FBO & depth texture for shadows
    // --------------------------------------------------
    static const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Tell the final-pass shader which texture units to sample from
    shader.use();
    shader.setInt("diffuseTexture", 0);  // plane or bot’s color
    shader.setInt("depthMap", 1);        // depth map

    glm::vec3 lightPos(0.0f, 2.0f, 0.1f);
    bool shadows = true;





    glm::vec3 zero(0.0f, 0.0f, 0.0f);

    // --------------------------------------------------
    // 5) Create Your Objects
    // --------------------------------------------------











    std::vector<Chunk> chunks = generateChunks(shader, simpleDepthShader);

    chunksinitialise(chunks, shader, simpleDepthShader);



    MyBot bot;
    Skybox skybox("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/sky.png");
    Plane  plane;

    bot.initialize(shader, simpleDepthShader);
    skybox.initialize();
    plane.initialize(shader);

    // Setup some transforms
    glm::mat4 modelMatrix = glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        glm::vec3(0.01f, 1.0f, 0.01f)
    );

    // --------------------------------------------------
    // 6) Setup for Light Source (a Small Red Cube)
    // --------------------------------------------------

    // Generate and bind the VAO and VBO
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture Coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    // --------------------------------------------------
    // 7) Create a 1x1 Red Texture for the Light Cube
    // --------------------------------------------------

    // Generate texture ID
    unsigned int redTexture;
    glGenTextures(1, &redTexture);
    glBindTexture(GL_TEXTURE_2D, redTexture);

    // Define a single red pixel (RGBA)
    unsigned char redPixel[] = { 255, 0, 0, 255 };

    // Allocate and load the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, redPixel);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // --------------------------------------------------
    // 8) Main Loop
    // --------------------------------------------------
    float  fTime = 0.0f;
    unsigned long frames = 0;
    double lastTime = glfwGetTime();


    float near_plane = 0.0f;
    float far_plane = 1000.0f;
    // --------------------------------------------------
// 9) Create Shaders for Depth Visualization
// --------------------------------------------------
    Shader depthShader(
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/depth_visualize.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/depth_visualize.frag"
    );
    depthShader.use();
    depthShader.setInt("depthMap", 1); // Texture unit 1
    depthShader.setFloat("near_plane", near_plane); // Ensure near_plane is defined
    depthShader.setFloat("far_plane", far_plane);   // Ensure far_plane is defined

    // --------------------------------------------------
// 10) Setup Quad for Depth Map Visualization
// --------------------------------------------------
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // Texture Coordinates attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0); // Unbind VAO

    glm::mat4 redCubeModel = glm::mat4(1.0f);
    redCubeModel = glm::translate(redCubeModel, glm::vec3(6.0f, 0.0f, 0.0f)); // Translate to (6, 0, 0)
    redCubeModel = glm::scale(redCubeModel, glm::vec3(0.2f)); // Scale cube for visibility

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------
        // Per-frame time logic
        // ----------------------------------------------
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
        fTime += deltaTime;
        frames++;

        if (fTime > 2.0f) {
            float fps = frames / fTime;
            frames = 0;
            fTime = 0.0f;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2)
                << "Shadow Mapping | FPS: " << fps;
            glfwSetWindowTitle(window, stream.str().c_str());
        }

        // Update light position (make it dynamic)
        lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 6.0);



        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // Also compute camera's view/projection
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(FoV),
            float(windowWidth) / float(windowHeight),
            zNear,
            zFar
        );
        glm::mat4 viewMatrix = glm::lookAt(eye_center, lookat, up);

        // We'll use vp = projectionMatrix * viewMatrix for plane
        glm::mat4 vp = projectionMatrix * viewMatrix;

        // ----------------------------------------------
        // 2) FIRST PASS: Render scene from light's POV
        //    into the depth map
        // ----------------------------------------------

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        shader.setMat4("model", redCubeModel);
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        //for (unsigned int i = 0; i < 6; ++i) {
        //    std::cout << "Shadow Matrix " << i << ": " << glm::to_string(shadowTransforms[i]) << std::endl;
        //}
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPos);

        // Render each object with its own "shadowRender" method
        //bot.shadowRender(simpleDepthShader, depthMapFBO);





        chunksshadowrender(chunks, simpleDepthShader, depthMapFBO);







        //plane.shadowRender(simpleDepthShader, depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ----------------------------------------------
        // 3) SECOND PASS: Render scene as normal
        //    from the camera’s POV, but sample shadowMap
        // ----------------------------------------------
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // a) Bind the final-pass shader
        shader.use();
        shader.setInt("diffuseTexture", 0);  // plane or bot’s color
        shader.setInt("depthMap", 1);        // depth map

        shader.setMat4("projection", projectionMatrix);
        shader.setMat4("view", viewMatrix);

        // c) Also set "lightPos" and "viewPos"
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", eye_center);
        shader.setInt("shadows", shadows);
        shader.setFloat("far_plane", far_plane);

        // d) Now bind the shadow map to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        // e) Render each object in normal camera space
        //bot.render(shader, vp, projectionMatrix, viewMatrix, true, lightPos, zero);
        //plane.render(shader, vp, modelMatrix, eye_center, true, lightPos, zero);


        renderRedCubeAtPosition(shader, lightVAO, redTexture, viewMatrix, projectionMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
        chunksrender(chunks, shader, vp, projectionMatrix, viewMatrix, shadows, lightPos, modelMatrix, zero);



        








        // --------------------------------------------------
        // 4. Render the Red Cube at the Light Source
        // --------------------------------------------------

        // Use the existing shader program
        shader.use();

        // Create transformations for the light cube
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos); // Move cube to light position
        lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // Scale cube for visibility

        // Set the transformation matrices in the shader
        shader.setMat4("model", lightModel);
        shader.setMat4("view", viewMatrix);
        shader.setMat4("projection", projectionMatrix);

        // Bind the red texture to texture unit 0 (diffuseTexture)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, redTexture);
        shader.setInt("diffuseTexture", 0); // Ensure diffuseTexture sampler is set to texture unit 0

        // Render the cube
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices for a cube
        glBindVertexArray(0);

        // Unbind the texture after rendering
        glBindTexture(GL_TEXTURE_2D, 0);

        // --------------------------------------------------
        // 5. Render the Skybox
        // --------------------------------------------------
        glDepthMask(GL_FALSE); // Disable depth write
        skybox.render(viewMatrix, projectionMatrix);
        glDepthMask(GL_TRUE);  // Re-enable depth write
        //skybox.render(viewMatrix, projectionMatrix);






        // --------------------------------------------------
// 11) Render Depth Map Visualization
// --------------------------------------------------

// Set the viewport to a smaller region, e.g., top-right corner
        int quadWidth = SHADOW_WIDTH / 4;  // Adjust size as needed
        int quadHeight = SHADOW_HEIGHT / 4;
        glViewport(windowWidth - quadWidth - 10, windowHeight - quadHeight - 10, quadWidth, quadHeight);

        // Disable depth testing and face culling for the quad
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Use the depth visualization shader
        depthShader.use();

        // Set the face to visualize (0: POSITIVE_X, 1: NEGATIVE_X, 2: POSITIVE_Y, 3: NEGATIVE_Y, 4: POSITIVE_Z, 5: NEGATIVE_Z)
        int currentFace = 0; // Change this value to visualize different faces
        // Set the face to visualize based on user input
        depthShader.setInt("face", currentDepthFace);

        //depthShader.setInt("face", currentFace);

        // Bind the depth cubemap to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        // Render the quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Re-enable depth testing and face culling
        glEnable(GL_DEPTH_TEST);

        // Restore the default viewport
        glViewport(0, 0, windowWidth, windowHeight);








        // Finally, swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteTextures(1, &redTexture);

    glfwTerminate();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    glm::vec3 forward = glm::normalize(lookat - eye_center);

    // Calculate right (perpendicular direction to both 'up' and 'forward')
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Move forward (along the 'lookat' direction)
        eye_center += cameraSpeed * forward;
        lookat += cameraSpeed * forward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Move left (perpendicular to the 'lookat' direction)
        eye_center += cameraSpeed * right;
        lookat += cameraSpeed * right;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Move backward (opposite of the 'lookat' direction)
        eye_center -= cameraSpeed * forward;
        lookat -= cameraSpeed * forward;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // Move right (perpendicular to the 'lookat' direction)
        eye_center -= cameraSpeed * right;
        lookat -= cameraSpeed * right;
    }
    //if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    //{
    //	playbackSpeed += 1.0f;
    //	if (playbackSpeed > 10.0f)
    //		playbackSpeed = 10.0f;
    //}

    //if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    //{
    //	playbackSpeed -= 1.0f;
    //	if (playbackSpeed < 1.0f) {
    //		playbackSpeed = 1.0f;
    //	}
    //}

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  // Left arrow key
    {
        // Rotate the camera around the Y-axis (counterclockwise)
        float angle = 0.05f; // Set the rotation speed (radians)

        // Calculate new position for the camera
        float x = eye_center.x * cos(angle) - eye_center.z * sin(angle);
        float z = eye_center.x * sin(angle) + eye_center.z * cos(angle);

        // Update eye_center (camera position)
        eye_center.x = x;
        eye_center.z = z;

        // Update the lookat direction to always point to the center of the object
        lookat = glm::normalize(glm::vec3(-eye_center.x, 0.0f, -eye_center.z));
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)  // Right arrow key
    {
        // Rotate the camera around the Y-axis (clockwise)
        float angle = -0.05f; // Negative for clockwise

        // Calculate new position for the camera
        float x = eye_center.x * cos(angle) - eye_center.z * sin(angle);
        float z = eye_center.x * sin(angle) + eye_center.z * cos(angle);

        // Update eye_center (camera position)
        eye_center.x = x;
        eye_center.z = z;

        // Update the lookat direction to always point to the center of the object
        lookat = glm::normalize(glm::vec3(-eye_center.x, 0.0f, -eye_center.z));
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        //playAnimation = !playAnimation;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        // Move upward (along the 'up' vector)
        eye_center += cameraSpeed * up;
        lookat += cameraSpeed * up;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        // Move downward (opposite to the 'up' vector)
        eye_center -= cameraSpeed * up;
        lookat -= cameraSpeed * up;
    }
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1)
            currentDepthFace = 0; // POSITIVE_X
        if (key == GLFW_KEY_2)
            currentDepthFace = 1; // NEGATIVE_X
        if (key == GLFW_KEY_3)
            currentDepthFace = 2; // POSITIVE_Y
        if (key == GLFW_KEY_4)
            currentDepthFace = 3; // NEGATIVE_Y
        if (key == GLFW_KEY_5)
            currentDepthFace = 4; // POSITIVE_Z
        if (key == GLFW_KEY_6)
            currentDepthFace = 5; // NEGATIVE_Z
    }
}






