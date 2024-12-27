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
    // Shader variable IDs
    GLuint mvpMatrixID;
    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;

    // GLTF data
    tinygltf::Model model;

    // Transforms
    std::vector<glm::mat4> nodeTransforms;
    std::vector<glm::mat4> localTransforms;
    std::vector<glm::mat4> globalTransforms;

    // Each VAO corresponds to each mesh primitive in the GLTF model
    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
        GLuint index;
        GLuint texture;
        GLuint normal;
        GLuint position;
        GLuint textureID;
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

    // Loads a GLTF model from file
    bool loadModel(tinygltf::Model& model, const char* filename);

    // Set rotation in degrees on a tinygltf::Node
    void setRotationInDegrees(tinygltf::Node& node, float angleX, float angleY, float angleZ);

    // Initialize the bot (load model, prepare buffers, etc.)
    void initialize();

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
    void render(glm::mat4 cameraMatrix, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

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

private:
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
        int nodeIndex);

    // The core draw call for a mesh
    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
        tinygltf::Model& model,
        const glm::mat4& vp);

    // A helper to draw an individual mesh
    void drawMesh(const std::vector<PrimitiveObject>& primitiveObjects,
        int primitiveIndex,
        tinygltf::Model& model,
        tinygltf::Mesh& mesh);
};


//--------------------------------------------------------
// CALLBACK FUNCTION
//--------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

#endif // LOADER_H
