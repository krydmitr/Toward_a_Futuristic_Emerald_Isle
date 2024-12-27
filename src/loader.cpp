#include "loader.h"

// Keep these includes if they are needed by the .cpp implementation.
// Because "loader.h" might already include them, you can remove duplicates if desired,
// but here we'll keep them so as not to remove anything you already had.
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

////////////////////////////////
#include <iomanip>
#include <glm/gtx/quaternion.hpp>
////////////////////////////////

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// --------------------------------------
// Definitions for global variables
// (they were extern in loader.h)
// --------------------------------------
GLFWwindow* window = nullptr;
int windowWidth = 1024;
int windowHeight = 768;

float cameraSpeed = 1;                // Removed "static"
glm::vec3 eye_center = glm::vec3(4.0f, 0.0f, -5.0f);
glm::vec3 lookat = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
float FoV = 45.0f;
float zNear = 0.1f;
float zFar = 1200.0f;

glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

// Animation
bool playAnimation = true;
float playbackSpeed = 2.0f;


// ---------------------------------------------------------
// Implementation of MyBot methods (the struct is in loader.h)
// ---------------------------------------------------------

// getNodeTransform
glm::mat4 MyBot::getNodeTransform(const tinygltf::Node& node) {
	// DO NOT REMOVE ANY COMMENTS
	glm::mat4 transform(1.0f);

	if (node.matrix.size() == 16) {
		transform = glm::make_mat4(node.matrix.data());
	}
	else {
		if (node.translation.size() == 3) {
			transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
		}
		if (node.rotation.size() == 4) {
			glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			transform *= glm::mat4_cast(q);
		}
		if (node.scale.size() == 3) {
			transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
		}
	}
	return transform;
}

// computeLocalNodeTransform
void MyBot::computeLocalNodeTransform(const tinygltf::Model& model,
	int nodeIndex,
	std::vector<glm::mat4>& localTransforms)
{
	const tinygltf::Node& node = model.nodes[nodeIndex];
	localTransforms[nodeIndex] = getNodeTransform(node);
	for (int child : node.children) {
		computeLocalNodeTransform(model, child, localTransforms);
	}
}

// computeGlobalNodeTransform
void MyBot::computeGlobalNodeTransform(const tinygltf::Model& model,
	const std::vector<glm::mat4>& localTransforms,
	int nodeIndex,
	const glm::mat4& parentTransform,
	std::vector<glm::mat4>& globalTransforms)
{
	// DO NOT REMOVE ANY COMMENTS
	glm::mat4 global = parentTransform * localTransforms[nodeIndex];
	globalTransforms[nodeIndex] = global;

	const tinygltf::Node& node = model.nodes[nodeIndex];
	for (auto childIndex : node.children) {
		computeGlobalNodeTransform(model, localTransforms, childIndex, global, globalTransforms);
	}
}

// prepareSkinning
std::vector<MyBot::SkinObject> MyBot::prepareSkinning(const tinygltf::Model& model) {
	// DO NOT REMOVE ANY COMMENTS
	std::vector<SkinObject> skinObjects;

	for (size_t i = 0; i < model.skins.size(); i++) {
		SkinObject skinObject;

		const tinygltf::Skin& skin = model.skins[i];

		// Read inverseBindMatrices
		const tinygltf::Accessor& accessor = model.accessors[skin.inverseBindMatrices];
		assert(accessor.type == TINYGLTF_TYPE_MAT4);
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		const float* ptr = reinterpret_cast<const float*>(
			buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

		skinObject.inverseBindMatrices.resize(accessor.count);
		for (size_t j = 0; j < accessor.count; j++) {
			float m[16];
			memcpy(m, ptr + j * 16, 16 * sizeof(float));
			skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
		}

		assert(skin.joints.size() == accessor.count);

		skinObject.globalJointTransforms.resize(skin.joints.size());
		skinObject.jointMatrices.resize(skin.joints.size());

		// ----------------------------------------------
		// TODO: your code here to compute joint matrices
		// Compute node transforms for all nodes
		std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
		for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
			computeLocalNodeTransform(model, (int)nodeIndex, localTransforms);
		}

		// Compute global transforms starting from the root nodes
		std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));
		const tinygltf::Scene& scene = model.scenes[model.defaultScene];
		for (auto rootNodeIndex : scene.nodes) {
			computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, glm::mat4(1.0f), globalTransforms);
		}

		// Compute joint matrices: globalTransformOfJoint * inverseBindMatrix
		for (size_t j = 0; j < skin.joints.size(); j++) {
			int jointNodeIndex = skin.joints[j];
			glm::mat4 globalJointTransform = globalTransforms[jointNodeIndex];
			skinObject.globalJointTransforms[j] = globalJointTransform;
			skinObject.jointMatrices[j] = globalJointTransform * skinObject.inverseBindMatrices[j];
		}
		// ----------------------------------------------


		// Repeat partial steps (as in your original code)
		for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
			computeLocalNodeTransform(model, (int)0, localTransforms);
		}
		for (auto rootNodeIndex : scene.nodes) {
			computeGlobalNodeTransform(model, localTransforms, 0, glm::mat4(1.0f), globalTransforms);
		}
		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject& innerSkinObject = skinObjects[i];
			const tinygltf::Skin& innerSkin = model.skins[i];
			for (size_t j = 0; j < innerSkin.joints.size(); j++) {
				int jointNodeIndex = innerSkin.joints[j];
				glm::mat4 globalJointTransform = globalTransforms[jointNodeIndex];
				innerSkinObject.globalJointTransforms[j] = globalJointTransform;
				innerSkinObject.jointMatrices[j] = globalJointTransform *
					innerSkinObject.inverseBindMatrices[j];
			}
		}
		skinObjects.push_back(skinObject);
	}
	return skinObjects;
}

// findKeyframeIndex
int MyBot::findKeyframeIndex(const std::vector<float>& times, float animationTime)
{
	// DO NOT REMOVE ANY COMMENTS
	int left = 0;
	int right = (int)times.size() - 1;

	while (left <= right) {
		int mid = (left + right) / 2;

		if (mid + 1 < (int)times.size() &&
			times[mid] <= animationTime && animationTime < times[mid + 1]) {
			return mid;
		}
		else if (times[mid] > animationTime) {
			right = mid - 1;
		}
		else { // animationTime >= times[mid + 1]
			left = mid + 1;
		}
	}

	return (int)times.size() - 2;
}

// prepareAnimation
std::vector<MyBot::AnimationObject> MyBot::prepareAnimation(const tinygltf::Model& model)
{
	// DO NOT REMOVE ANY COMMENTS
	std::vector<AnimationObject> animationObjects;
	for (const auto& anim : model.animations) {
		AnimationObject animationObject;

		for (const auto& sampler : anim.samplers) {
			SamplerObject samplerObject;

			const tinygltf::Accessor& inputAccessor = model.accessors[sampler.input];
			const tinygltf::BufferView& inputBufferView = model.bufferViews[inputAccessor.bufferView];
			const tinygltf::Buffer& inputBuffer = model.buffers[inputBufferView.buffer];

			assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

			// Input (time) values
			samplerObject.input.resize(inputAccessor.count);

			const unsigned char* inputPtr =
				&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
			int stride = inputAccessor.ByteStride(inputBufferView);
			for (size_t i = 0; i < inputAccessor.count; ++i) {
				samplerObject.input[i] =
					*reinterpret_cast<const float*>(inputPtr + i * stride);
			}

			const tinygltf::Accessor& outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView& outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer& outputBuffer = model.buffers[outputBufferView.buffer];

			assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

			const unsigned char* outputPtr =
				&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			int outputStride = outputAccessor.ByteStride(outputBufferView);

			// Output values
			samplerObject.output.resize(outputAccessor.count);

			for (size_t i = 0; i < outputAccessor.count; ++i) {
				if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
					memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
				}
				else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
					memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
				}
				else {
					std::cout << "Unsupport accessor type ..." << std::endl;
				}
			}
			animationObject.samplers.push_back(samplerObject);
		}
		animationObjects.push_back(animationObject);
	}
	return animationObjects;
}

// updateAnimation
void MyBot::updateAnimation(const tinygltf::Model& model,
	const tinygltf::Animation& anim,
	const AnimationObject& animationObject,
	float time,
	std::vector<glm::mat4>& nodeTransforms)
{
	// DO NOT REMOVE ANY COMMENTS
	for (const auto& channel : anim.channels) {
		int targetNodeIndex = channel.target_node;
		const auto& sampler = anim.samplers[channel.sampler];

		const tinygltf::Accessor& outputAccessor = model.accessors[sampler.output];
		const tinygltf::BufferView& outputBufferView = model.bufferViews[outputAccessor.bufferView];
		const tinygltf::Buffer& outputBuffer = model.buffers[outputBufferView.buffer];

		// Calculate current animation time (wrap if necessary)
		const std::vector<float>& times = animationObject.samplers[channel.sampler].input;
		float animationTime = fmod(time, times.back());

		// TODO: Find a keyframe for getting animation data
		int keyframeIndex = findKeyframeIndex(times, animationTime);

		const unsigned char* outputPtr =
			&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];

		// TODO: Add interpolation for smooth animation
		float t0 = times[keyframeIndex];
		float t1 = times[keyframeIndex + 1];
		float alpha = (animationTime - t0) / (t1 - t0);

		const glm::vec4& v0 = animationObject.samplers[channel.sampler].output[keyframeIndex];
		const glm::vec4& v1 = animationObject.samplers[channel.sampler].output[keyframeIndex + 1];

		if (channel.target_path == "translation") {
			glm::vec3 translation = glm::mix(glm::vec3(v0), glm::vec3(v1), alpha);
			nodeTransforms[targetNodeIndex] = glm::translate(glm::mat4(1.0f), translation);
		}
		else if (channel.target_path == "rotation") {
			glm::quat q0 = glm::quat(v0.w, v0.x, v0.y, v0.z);
			glm::quat q1 = glm::quat(v1.w, v1.x, v1.y, v1.z);
			glm::quat rotation = glm::slerp(q0, q1, alpha);
			nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
		}
		else if (channel.target_path == "scale") {
			glm::vec3 scale = glm::mix(glm::vec3(v0), glm::vec3(v1), alpha);
			nodeTransforms[targetNodeIndex] = glm::scale(glm::mat4(1.0f), scale);
		}
	}
}

// updateSkinning
void MyBot::updateSkinning(const std::vector<glm::mat4>& nodeTransforms,
	std::vector<SkinObject>& skinObjects)
{
	// DO NOT REMOVE ANY COMMENTS
	for (auto& skinObject : skinObjects) {
		for (size_t i = 0; i < skinObject.globalJointTransforms.size(); ++i) {
			skinObject.jointMatrices[i] =
				nodeTransforms[i] * skinObject.inverseBindMatrices[i];
		}
	}
}

// update
void MyBot::update(float time) {
	// DO NOT REMOVE ANY COMMENTS
	/*
	   const auto& anim = model.animations[0];
	   updateAnimation(model, anim, animationObjects[0], time, nodeTransforms);
	   updateSkinning(nodeTransforms, skinObjects);
	*/
}

// loadModel
bool MyBot::loadModel(tinygltf::Model& model, const char* filename) {
	// DO NOT REMOVE ANY COMMENTS
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

// setRotationInDegrees
void MyBot::setRotationInDegrees(tinygltf::Node& node,
	float angleX,
	float angleY,
	float angleZ)
{
	// DO NOT REMOVE ANY COMMENTS
	float radX = glm::radians(angleX);
	float radY = glm::radians(angleY);
	float radZ = glm::radians(angleZ);

	glm::quat rotation = glm::quat(glm::vec3(radX, radY, radZ));
	node.rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
}

// initialize
void MyBot::initialize() {
	// Modify your path if needed
	//if (!loadModel(model, "./lab4/model/bot/bot.gltf")) {
	//if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/model/bot/bot.gltf")) {
	//if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/rubber_duck_toy_1k/rubber_duck_toy_1k.gltf")) {
	//if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/covered_car_1k/covered_car_1k.gltf")) {
	//if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/antique_estoc_1k/antique_estoc_1k.gltf")) {
	if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/strawberry_chocolate_cake_1k/strawberry_chocolate_cake_1k.gltf")) {
		//if (!loadModel(model, "C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/Camera_01_1k/Camera_01_1k.gltf")) {
		return;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	if (!scene.nodes.empty()) {
		int rootNodeIndex = scene.nodes[0]; // For example, take the first node as root
		tinygltf::Node& rootNode = model.nodes[rootNodeIndex];
		// Set a new translation for the root node
		//rootNode.translation = { 0.0f, 2.0f, .0f };  // Move the model 1 unit along X
		// You can also modify rotation and scale if needed:
		//rootNode.rotation = { x, y, z, w };
		//rootNode.rotation = { 0, 0.3827f, 0, 1.0f };
		//rootNode.rotation = { 1, 0, 0, 1 };
		//rootNode.scale = { 3, 2, 1 };
		//setRotationInDegrees(rootNode, 45.0f, 30.0f, 90.0f);
	}

	localTransforms.resize(model.nodes.size(), glm::mat4(1.0f));
	globalTransforms.resize(model.nodes.size(), glm::mat4(1.0f));

	for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
		computeLocalNodeTransform(model, (int)nodeIndex, localTransforms);
	}

	for (auto rootNodeIndex : scene.nodes) {
		computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, glm::mat4(1.0f), globalTransforms);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Prepare buffers for rendering
	primitiveObjects = bindModel(model);

	// Prepare animation data
	animationObjects = prepareAnimation(model);

	// Create and compile our GLSL program from the shaders
	programID = LoadShadersFromFile(
		"C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/"
		"lab4/shader/bot.vert",
		"C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/"
		"lab4/shader/bot.frag");

	if (programID == 0)
	{
		std::cerr << "Failed to load shaders." << std::endl;
	}

	// Get a handle for GLSL variables
	mvpMatrixID = glGetUniformLocation(programID, "MVP");
	lightPositionID = glGetUniformLocation(programID, "lightPosition");
	lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
}

// bindMesh
void MyBot::bindMesh(std::vector<PrimitiveObject>& primitiveObjects,
	tinygltf::Model& model,
	tinygltf::Mesh& mesh)
{
	// DO NOT REMOVE ANY COMMENTS
	std::map<int, GLuint> vbos;
	for (size_t i = 0; i < model.bufferViews.size(); ++i) {
		const tinygltf::BufferView& bufferView = model.bufferViews[i];
		int target = bufferView.target;

		if (bufferView.target == 0) {
			// ...
			continue;
		}

		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(target, vbo);
		glBufferData(target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

		vbos[i] = vbo;
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		PrimitiveObject primitiveObject;
		glGenVertexArrays(1, &primitiveObject.vao);
		glBindVertexArray(primitiveObject.vao);

		tinygltf::Accessor indexAccessor = model.accessors[primitive.attributes["INDEX"]];
		tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];

		glGenBuffers(1, &primitiveObject.index);
		glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.index);
		glBufferData(GL_ARRAY_BUFFER, indexBufferView.byteLength,
			&model.buffers[indexBufferView.buffer].data.at(0) + indexBufferView.byteOffset,
			GL_STATIC_DRAW);

		if (primitive.indices >= 0) {
			const tinygltf::Accessor& idxAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& idxBufferView = model.bufferViews[idxAccessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[idxBufferView.buffer];

			GLuint indexBuffer;
			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBufferView.byteLength,
				buffer.data.data() + idxBufferView.byteOffset, GL_STATIC_DRAW);
			primitiveObject.index = indexBuffer;
		}

		tinygltf::Accessor normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
		tinygltf::BufferView normalBufferView = model.bufferViews[normalAccessor.bufferView];
		glGenBuffers(1, &primitiveObject.normal);
		glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.normal);
		glBufferData(GL_ARRAY_BUFFER, normalBufferView.byteLength,
			&model.buffers[normalBufferView.buffer].data.at(0) +
			normalBufferView.byteOffset,
			GL_STATIC_DRAW);

		tinygltf::Accessor positionAccessor = model.accessors[primitive.attributes["POSITION"]];
		tinygltf::BufferView positionBufferView = model.bufferViews[positionAccessor.bufferView];
		glGenBuffers(1, &primitiveObject.position);
		glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.position);
		glBufferData(GL_ARRAY_BUFFER, positionBufferView.byteLength,
			&model.buffers[positionBufferView.buffer].data.at(0) +
			positionBufferView.byteOffset,
			GL_STATIC_DRAW);

		tinygltf::Accessor textureAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
		tinygltf::BufferView textureBufferView = model.bufferViews[textureAccessor.bufferView];
		glGenBuffers(1, &primitiveObject.texture);
		glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.texture);
		glBufferData(GL_ARRAY_BUFFER, textureBufferView.byteLength,
			&model.buffers[textureBufferView.buffer].data.at(0) +
			textureBufferView.byteOffset,
			GL_STATIC_DRAW);

		primitiveObject.vbos = vbos;

		int matIndex = primitive.material;
		if (matIndex >= 0 && matIndex < (int)model.materials.size()) {
			tinygltf::Material& mat = model.materials[matIndex];
			if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
				int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
				if (textureIndex >= 0 && textureIndex < (int)model.textures.size()) {
					tinygltf::Texture& tex = model.textures[textureIndex];
					if (tex.source >= 0 && tex.source < (int)model.images.size()) {
						tinygltf::Image& image = model.images[tex.source];

						GLuint texid;
						glGenTextures(1, &texid);
						glBindTexture(GL_TEXTURE_2D, texid);
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

						GLenum format = GL_RGBA;
						if (image.component == 1) format = GL_RED;
						else if (image.component == 2) format = GL_RG;
						else if (image.component == 3) format = GL_RGB;

						GLenum type = GL_UNSIGNED_BYTE;
						if (image.bits == 16) type = GL_UNSIGNED_SHORT;

						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
							image.width, image.height, 0,
							format, type, &image.image.at(0));
						glGenerateMipmap(GL_TEXTURE_2D);

						primitiveObject.textureID = texid;
					}
				}
				else {
					primitiveObject.textureID = 0;
				}
			}
			else {
				primitiveObject.textureID = 0;
			}
		}
		else {
			primitiveObject.textureID = 0;
		}

		primitiveObjects.push_back(primitiveObject);
		glBindVertexArray(0);
	}
}

// bindModelNodes
void MyBot::bindModelNodes(std::vector<PrimitiveObject>& primitiveObjects,
	tinygltf::Model& model,
	tinygltf::Node& node)
{
	// DO NOT REMOVE ANY COMMENTS
	if ((node.mesh >= 0) && (node.mesh < (int)model.meshes.size())) {
		bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
	}
	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) &&
			(node.children[i] < (int)model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
	}
}

// bindModel
std::vector<MyBot::PrimitiveObject> MyBot::bindModel(tinygltf::Model& model) {
	// DO NOT REMOVE ANY COMMENTS
	std::vector<PrimitiveObject> primitiveObjects;
	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < (int)model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
	}
	return primitiveObjects;
}

// drawMesh
void MyBot::drawMesh(const std::vector<PrimitiveObject>& primitiveObjects,
	int primitiveIndex,
	tinygltf::Model& model,
	tinygltf::Mesh& mesh)
{
	// DO NOT REMOVE ANY COMMENTS
	glUseProgram(programID);
	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		const PrimitiveObject& primitiveObject = primitiveObjects[primitiveIndex + i];
		if (!glIsVertexArray(primitiveObject.vao)) {
			std::cerr << "Error: Invalid VAO for primitive index " << primitiveIndex << std::endl;
			return;
		}
		glBindVertexArray(primitiveObject.vao);

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, primitiveObjects[primitiveIndex + i].textureID);
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

		if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.position);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.normal);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.texture);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (primitive.indices >= 0) {
			const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitiveObject.index);
			glDrawElements(primitive.mode, indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));
		}
		else {
			const tinygltf::Accessor& positionAccessor =
				model.accessors[primitive.attributes.at("POSITION")];
			glDrawArrays(primitive.mode, 0, positionAccessor.count);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	glBindVertexArray(0);
}

// drawModelNodes
void MyBot::drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
	tinygltf::Model& model,
	tinygltf::Node& node,
	const glm::mat4& vp,
	const glm::mat4& parentMatrix,
	int nodeIndex)
{
	// DO NOT REMOVE ANY COMMENTS
	glm::mat4 modelMatrix = parentMatrix * globalTransforms[nodeIndex];
	glm::mat4 mvp = vp * modelMatrix;

	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, glm::value_ptr(mvp));

	if ((node.mesh >= 0) && (node.mesh < (int)model.meshes.size())) {
		tinygltf::Mesh& mesh = model.meshes[node.mesh];
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			drawMesh(primitiveObjects, node.mesh, model, mesh);
		}
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) &&
			(node.children[i] < (int)model.nodes.size()));
		drawModelNodes(primitiveObjects,
			model,
			model.nodes[node.children[i]],
			vp,
			modelMatrix,
			(int)i);
	}
}

// drawModel
void MyBot::drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
	tinygltf::Model& model,
	const glm::mat4& vp)
{
	// DO NOT REMOVE ANY COMMENTS
	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		drawModelNodes(primitiveObjects,
			model,
			model.nodes[scene.nodes[i]],
			vp,
			glm::mat4(1.0f),
			(int)i);
	}
}

// render
void MyBot::render(glm::mat4 cameraMatrix,
	const glm::mat4& projectionMatrix,
	const glm::mat4& viewMatrix)
{
	// DO NOT REMOVE ANY COMMENTS
	glUseProgram(programID);

	glm::mat4 mvp = cameraMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	glUniform3fv(lightPositionID, 1, &lightPosition[0]);
	glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

	for (auto& pObj : primitiveObjects) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pObj.textureID);
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
	}

	drawModel(primitiveObjects, model, projectionMatrix * viewMatrix);
}

// cleanup
void MyBot::cleanup() {
	// DO NOT REMOVE ANY COMMENTS
	glDeleteProgram(programID);
}

// ---------------------------------------------------------
// The rest of your code: main(), key_callback, etc. remain
// ---------------------------------------------------------

// Because we now have a global 'key_callback' declared in loader.h (as extern),
// we define it here:
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// DO NOT REMOVE ANY COMMENTS
	glm::vec3 forward = glm::normalize(lookat - eye_center);

	glm::vec3 right = glm::normalize(glm::cross(up, forward));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		eye_center += cameraSpeed * forward;
		lookat += cameraSpeed * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		eye_center += cameraSpeed * right;
		lookat += cameraSpeed * right;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		eye_center -= cameraSpeed * forward;
		lookat -= cameraSpeed * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		eye_center -= cameraSpeed * right;
		lookat -= cameraSpeed * right;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		playbackSpeed += 1.0f;
		if (playbackSpeed > 10.0f)
			playbackSpeed = 10.0f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		playbackSpeed -= 1.0f;
		if (playbackSpeed < 1.0f) {
			playbackSpeed = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		float angle = 0.05f;
		float x = eye_center.x * cos(angle) - eye_center.z * sin(angle);
		float z = eye_center.x * sin(angle) + eye_center.z * cos(angle);
		eye_center.x = x;
		eye_center.z = z;
		lookat = glm::normalize(glm::vec3(-eye_center.x, 0.0f, -eye_center.z));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		float angle = -0.05f;
		float x = eye_center.x * cos(angle) - eye_center.z * sin(angle);
		float z = eye_center.x * sin(angle) + eye_center.z * cos(angle);
		eye_center.x = x;
		eye_center.z = z;
		lookat = glm::normalize(glm::vec3(-eye_center.x, 0.0f, -eye_center.z));
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		playAnimation = !playAnimation;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
