#ifndef LOADER_H
#define LOADER_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <vector>
#include <string>

// A struct representing a 3D model loader and renderer
struct MyBot {
    GLuint mvpMatrixID;
    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;

    tinygltf::Model model;

    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
        GLuint index;
        GLuint texture;
        GLuint normal;
        GLuint position;
        GLuint textureID;
    };
    struct SkinObject {
        std::vector<glm::mat4> inverseBindMatrices;
        std::vector<glm::mat4> globalJointTransforms;
        std::vector<glm::mat4> jointMatrices;
    };
    struct AnimationObject {
        struct SamplerObject {
            std::vector<float> input;
            std::vector<glm::vec4> output;
            int interpolation;
        };
        std::vector<SamplerObject> samplers;
    };

    std::vector<glm::mat4> nodeTransforms;
    std::vector<glm::mat4> localTransforms;
    std::vector<glm::mat4> globalTransforms;
    std::vector<PrimitiveObject> primitiveObjects;
    std::vector<std::vector<PrimitiveObject>> meshPrimitiveObjects;
    std::vector<SkinObject> skinObjects;
    std::vector<AnimationObject> animationObjects;

    // Functions for loading and rendering models
    bool loadModel(tinygltf::Model& model, const char* filename);
    void initialize();
    void update(float time);
    void render(glm::mat4 cameraMatrix, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
    void cleanup();

    // Helper functions
    glm::mat4 getNodeTransform(const tinygltf::Node& node);
    void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4>& localTransforms);
    void computeGlobalNodeTransform(const tinygltf::Model& model, const std::vector<glm::mat4>& localTransforms, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4>& globalTransforms);

    std::vector<SkinObject> prepareSkinning(const tinygltf::Model& model);
    int findKeyframeIndex(const std::vector<float>& times, float animationTime);
    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model& model);
    void updateAnimation(const tinygltf::Model& model, const tinygltf::Animation& anim, const AnimationObject& animationObject, float time, std::vector<glm::mat4>& nodeTransforms);
    void updateSkinning(const std::vector<glm::mat4>& nodeTransforms, std::vector<SkinObject>& skinObjects);

    void bindMesh(std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Mesh& mesh);
    void bindModelNodes(std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Node& node);
    std::vector<PrimitiveObject> bindModel(tinygltf::Model& model);

    void drawMesh(const std::vector<PrimitiveObject>& primitiveObjects, int primitiveIndex, tinygltf::Model& model, tinygltf::Mesh& mesh);
    void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Node& node, const glm::mat4& vp, const glm::mat4& parentMatrix, int nodeIndex);
    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, const glm::mat4& vp);
};

#endif // LOADER_H
