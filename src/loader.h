#ifndef LOADER_H
#define LOADER_H

// System/standard headers
#include <map>
#include <vector>
#include <string>
#include <iostream>

// External library headers
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// tinygltf
#include <tiny_gltf.h>

// Local/project headers
#include <render/shader.h>  // Adjust to your actual shader header path

//--------------------------------------------------------
// GLOBAL VARIABLES (declared as extern here, but defined in loader.cpp)
//--------------------------------------------------------
extern GLFWwindow* window;
extern int windowWidth;
extern int windowHeight;

extern float cameraSpeed;
extern glm::vec3 eye_center;
extern glm::vec3 lookat;
extern glm::vec3 up;
extern float FoV;
extern float zNear;
extern float zFar;
extern glm::vec3 lightIntensity;
extern glm::vec3 lightPosition;

// Animation
extern bool playAnimation;
extern float playbackSpeed;


//--------------------------------------------------------
// STRUCT: MyBot
//--------------------------------------------------------
struct MyBot
{
    MyBot() {
        std::cout << "MyBot created with default constructor!" << std::endl;
    }

private:
    // Shader variable IDs
    GLuint mvpMatrixID;

    GLuint projection;
    GLuint shaderModel;

    GLuint modelMatrixID;

    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;
    GLuint shadowShaderProgram;
    //loadprogram
    //shadowloadprogram
    //shadowdepthloadprogram
    GLuint instanceVBO;
    
    Shader shader = Shader(
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.vert",
        "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows.frag"
    );
    //Shader shadowdepthloadprogram = Shader(
    //    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.vert",
    //    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.frag",
    //    "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/shader/point_shadows_depth.gs"
    //);
 
    // Shadows
    GLuint lightSpaceMatrixID; // For light-space matrix
    GLuint shadowMapID;        // For shadow map texture

    // GLTF data
    tinygltf::Model model;

    // Transforms
    std::vector<glm::mat4> nodeTransforms;
    std::vector<glm::mat4> localTransforms;
    std::vector<glm::mat4> globalTransforms;
    
    //unsigned int instanceVBO; // Instance VBO for transformation matrices
    std::vector<glm::mat4> instanceMatrices;
public:
    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
        GLuint index;
        GLuint texture;
        GLuint normal;
        GLuint position;
        GLuint textureID;
        int indexCount;
    };
    std::vector<PrimitiveObject> primitiveObjects;
    std::vector<std::vector<PrimitiveObject>> meshPrimitiveObjects;

    struct MeshPrimitiveRange {
        int startIndex;    // the offset in primitiveObjects
        int count;         // how many primitives this mesh has
    };
    std::vector<MeshPrimitiveRange> meshRange;  // one entry per mesh
    std::vector<bool> meshBound;

    // Skinning
    struct SkinObject {
        std::vector<glm::mat4> inverseBindMatrices;
        std::vector<glm::mat4> globalJointTransforms;
        std::vector<glm::mat4> jointMatrices;
    };
    std::vector<SkinObject> skinObjects;

    // Animation 
    struct SamplerObject {
        std::vector<float> input;
        std::vector<glm::vec4> output;
        int interpolation;
    };
    struct ChannelObject {
        int sampler;
        std::string targetPath;
        int targetNode;
    };
    struct AnimationObject {
        std::vector<SamplerObject> samplers;
    };
    std::vector<AnimationObject> animationObjects;
    
    //----------------------------------------------------
    // METHODS (declarations)
    //----------------------------------------------------
public:
    GLuint getVAO(int meshIndex, int primitiveIndex) const;

    // Loads a GLTF model from file
    bool loadModel(tinygltf::Model& model, const char* filename);

    // Set rotation in degrees on a tinygltf::Node
    void setRotationInDegrees(tinygltf::Node& node, float angleX, float angleY, float angleZ);

    // Initialize the bot (load model, prepare buffers, etc.)
    void initialize(Shader shader, Shader simpleDepthShader, int choice, int instanceCount);

    // Bind the entire GLTF model to your OpenGL buffers
    std::vector<PrimitiveObject> bindModel(tinygltf::Model& model);

    // Prepares animation data
    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model& model);

    // Update the node transforms for a given animation/time
    void updateAnimation(const tinygltf::Model& model,
        const tinygltf::Animation& anim,
        const AnimationObject& animationObject,
        float time,
        std::vector<glm::mat4>& nodeTransforms);

    // Called each frame (e.g., to apply animation updates)
    void update(float time);

    // Renders the bot
    //void render(Shader shader, const glm::mat4& vp, const glm::mat4& cameraMatrix, const glm::mat4& projectionMatrix,
    //    const glm::mat4& viewMatrix, bool shad, glm::vec3 lightPos, glm::vec3 position, glm::mat4& modelMatrix);
    void render(Shader shader, const glm::mat4& vp, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, bool shad, glm::vec3 lightPos, glm::vec3 position, glm::mat4 modelMatrix, int instanceCount);

    void MyBot::shadowRender(Shader simpleDepthShader, GLuint depthMapFBO, glm::vec3 position, int choice, int instanceCount);
    // Cleanup
    void cleanup();

    // --------- Utility for transform calculations -----------
    glm::mat4 getNodeTransform(const tinygltf::Node& node);

    void computeLocalNodeTransform(const tinygltf::Model& model,
        int nodeIndex,
        std::vector<glm::mat4>& localTransforms);

    void computeGlobalNodeTransform(const tinygltf::Model& model,
        const std::vector<glm::mat4>& localTransforms,
        int nodeIndex,
        const glm::mat4& parentTransform,
        std::vector<glm::mat4>& globalTransforms);

    std::vector<SkinObject> prepareSkinning(const tinygltf::Model& model);

    void updateSkinning(const std::vector<glm::mat4>& nodeTransforms, std::vector<SkinObject>& skinObjects);

    int findKeyframeIndex(const std::vector<float>& times, float animationTime);

    //void renderShadowMap(GLuint shadowMap);
    void MyBot::drawModelNodesDepth(Shader simpleDepthShader, const std::vector<PrimitiveObject>& primitives,
        tinygltf::Model& model,
        tinygltf::Node& node,
        const glm::mat4& parentTransform,
        int nodeIndex, glm::vec3 position, int choice, int instanceCount);

    void MyBot::drawModelDepth(Shader simpleDepthShader, const std::vector<PrimitiveObject>& primitives,
        tinygltf::Model& model, glm::vec3 position, int choice, int instanceCount);

    // A helper to bind a specific mesh within the model
    void bindMesh(std::vector<PrimitiveObject>& primitiveObjects,
        tinygltf::Model& model,
        tinygltf::Mesh& mesh, int meshIndex);

    // A helper to recursively bind all nodes
    void bindModelNodes(std::vector<PrimitiveObject>& primitiveObjects,
        tinygltf::Model& model,
        tinygltf::Node& node);

    // Draw all nodes (recursive)
    void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
        tinygltf::Model& model,
        tinygltf::Node& node,
        const glm::mat4& vp,
        const glm::mat4& parentMatrix,
        int nodeIndex, glm::vec3 position, glm::mat4 modelMatrix, int instanceCount);

    // The core draw call for a mesh
    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
        tinygltf::Model& model,
        const glm::mat4& vp, glm::vec3 position, glm::mat4 modelMatrix, int instanceCount);


    // A helper to draw an individual mesh
    void drawMesh(const std::vector<PrimitiveObject>& primitiveObjects,
        int primitiveIndex,
        tinygltf::Model& model,
        tinygltf::Mesh& mesh, int instanceCount);
};


//--------------------------------------------------------
// CALLBACK FUNCTION
//--------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

#endif // LOADER_H
