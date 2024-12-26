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

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static float cameraSpeed = 1;
// Camera
//static glm::vec3 eye_center(0.0f, 100.0f, 800.0f);
static glm::vec3 eye_center = glm::vec3(4.0f, 0.0f, -5.0f);

//static glm::vec3 lookat(0.0f, 0.0f, 0.0f);
static glm::vec3 lookat(0.0f, 0.0f, -1.0f);
static glm::vec3 up(0.0f, 1.0f, 0.0f);
static float FoV = 45.0f;
//static float FoV = 300.0f;
//static float zNear = 100.0f;
static float zNear = 0.1f;
//static float zNear = 60.0f;

static float zFar = 1200.0f; 

// Lighting  
static glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
static glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

// Animation 
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

struct MyBot {
	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint jointMatricesID;
	GLuint lightPositionID;
	GLuint lightIntensityID;
	GLuint programID;

	tinygltf::Model model;

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
	// Skinning 
	struct SkinObject {
		// Transforms the geometry into the space of the respective joint
		std::vector<glm::mat4> inverseBindMatrices;  

		// Transforms the geometry following the movement of the joints
		std::vector<glm::mat4> globalJointTransforms;

		// Combined transforms
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
		std::vector<SamplerObject> samplers;	// Animation data
	};
	std::vector<AnimationObject> animationObjects;

	glm::mat4 getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f); 

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
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
	
	//void computeLocalNodeTransform(const tinygltf::Model& model, 
	//	int nodeIndex, 
	//	std::vector<glm::mat4> &localTransforms)
	//{
	//	// ---------------------------------------
	//	// TODO: your code here
	//	const tinygltf::Node& node = model.nodes[nodeIndex];

	//	glm::mat4 transform(1.0f);

	//	if (node.matrix.size() == 16) {
	//		transform = glm::make_mat4(node.matrix.data());
	//	}
	//	else {
	//		// Start with identity
	//		transform = glm::mat4(1.0f);

	//		// Translation
	//		if (node.translation.size() == 3) {
	//			transform = glm::translate(transform,
	//				glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
	//		}

	//		// Rotation (quaternion)
	//		if (node.rotation.size() == 4) {
	//			glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
	//			transform *= glm::mat4_cast(q);
	//		}

	//		// Scale
	//		if (node.scale.size() == 3) {
	//			transform = glm::scale(transform,
	//				glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
	//		}
	//	}

	//	localTransforms[nodeIndex] = transform;
	//	// ---------------------------------------
	//}

	void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4>& localTransforms) {
		const tinygltf::Node& node = model.nodes[nodeIndex];
		localTransforms[nodeIndex] = getNodeTransform(node);
		for (int child : node.children) {
			computeLocalNodeTransform(model, child, localTransforms);
		}
	}


	void computeGlobalNodeTransform(const tinygltf::Model& model, 
		const std::vector<glm::mat4> &localTransforms,
		int nodeIndex, const glm::mat4& parentTransform, 
		std::vector<glm::mat4> &globalTransforms)
	{
		// ----------------------------------------
		// TODO: your code here
		glm::mat4 global = parentTransform * localTransforms[nodeIndex];
		globalTransforms[nodeIndex] = global;

		const tinygltf::Node& node = model.nodes[nodeIndex];
		for (auto childIndex : node.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, global, globalTransforms);
		}
		// ----------------------------------------
	}


	/*void computeGlobalNodeTransform(const tinygltf::Model& model,
		const std::vector<glm::mat4>& localTransforms,
		int nodeIndex, const glm::mat4& parentTransform,
		std::vector<glm::mat4>& globalTransforms)
	{
		globalTransforms[nodeIndex] = parentTransform * localTransforms[nodeIndex];
		const tinygltf::Node& node = model.nodes[nodeIndex];
		for (int child : node.children) {
			computeGlobalNodeTransform(model, localTransforms, child, globalTransforms[nodeIndex], globalTransforms);
		}
	}*/




	std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model) {
		std::vector<SkinObject> skinObjects;

		// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject skinObject;

			const tinygltf::Skin &skin = model.skins[i];

			// Read inverseBindMatrices
			const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
			assert(accessor.type == TINYGLTF_TYPE_MAT4);
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			const float *ptr = reinterpret_cast<const float *>(
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


				// Compute node transforms for all nodes
			//std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
			for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
				computeLocalNodeTransform(model, (int)0, localTransforms);
			}

			//std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));
			// We assume the scene's root nodes have no parents and parentTransform = identity
			//const tinygltf::Scene& scene = model.scenes[model.defaultScene];
			for (auto rootNodeIndex : scene.nodes) {
				computeGlobalNodeTransform(model, localTransforms, 0, glm::mat4(1.0f), globalTransforms);
			}

			for (size_t i = 0; i < model.skins.size(); i++) {
				SkinObject& skinObject = skinObjects[i];
				const tinygltf::Skin& skin = model.skins[i];

				// For each joint, jointMatrix = globalTransformOfJoint * inverseBindMatrix
				for (size_t j = 0; j < skin.joints.size(); j++) {
					int jointNodeIndex = skin.joints[j];
					glm::mat4 globalJointTransform = globalTransforms[jointNodeIndex];
					skinObject.globalJointTransforms[j] = globalJointTransform;
					skinObject.jointMatrices[j] = globalJointTransform * skinObject.inverseBindMatrices[j];
				}
			}


			// ----------------------------------------------

			skinObjects.push_back(skinObject);
		}
		return skinObjects;
	}

	int findKeyframeIndex(const std::vector<float>& times, float animationTime) 
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model) 
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;
			
			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}
				
				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);
				
				// Output values
				samplerObject.output.resize(outputAccessor.count);
				
				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);			
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void updateAnimation(
		const tinygltf::Model &model, 
		const tinygltf::Animation &anim, 
		const AnimationObject &animationObject, 
		float time,
		std::vector<glm::mat4> &nodeTransforms) 
		//std::vector<glm::mat4> nodeTransforms) 
	{
		// There are many channels so we have to accumulate the transforms 
		for (const auto &channel : anim.channels) {
			
			int targetNodeIndex = channel.target_node;
			const auto &sampler = anim.samplers[channel.sampler];
			
			// Access output (value) data for the channel
			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			// Calculate current animation time (wrap if necessary)
			const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
			float animationTime = fmod(time, times.back());
			
			// ----------------------------------------------------------
			// TODO: Find a keyframe for getting animation data 
			int keyframeIndex = findKeyframeIndex(times, animationTime);
			// ----------------------------------------------------------
			//int keyframeIndex = 0; 

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			// -----------------------------------------------------------
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
			// -----------------------------------------------------------
			/*if (channel.target_path == "translation") {
				glm::vec3 translation0, translation1;
				memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 translation = translation0;
				nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
			} else if (channel.target_path == "rotation") {
				glm::quat rotation0, rotation1;
				memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));

				glm::quat rotation = rotation0;
				nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
			} else if (channel.target_path == "scale") {
				glm::vec3 scale0, scale1;
				memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 scale = scale0;
				nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
			}*/
		}
	}

	void updateSkinning(const std::vector<glm::mat4>& nodeTransforms, std::vector<SkinObject>& skinObjects){

		// -------------------------------------------------
		// TODO: Recompute joint matrices 
		for (auto& skinObject : skinObjects) {
			for (size_t i = 0; i < skinObject.globalJointTransforms.size(); ++i) {
				//skinObject.jointMatrices[i] = nodeTransforms[skinObject.globalJointTransforms[i]] * skinObject.inverseBindMatrices[i];
				skinObject.jointMatrices[i] = nodeTransforms[i] * skinObject.inverseBindMatrices[i];

			}
		}

		// -------------------------------------------------
	}

	void update(float time) {

		// -------------------------------------------------
		// TODO: your code here
	/*	const auto& anim = model.animations[0];
		updateAnimation(model, anim, animationObjects[0], time, nodeTransforms);
		updateSkinning(nodeTransforms, skinObjects);*/

		// -------------------------------------------------

	}

	bool loadModel(tinygltf::Model &model, const char *filename) {
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

	void setRotationInDegrees(tinygltf::Node& node, float angleX, float angleY, float angleZ) {
		// Convert angles from degrees to radians
		float radX = glm::radians(angleX);
		float radY = glm::radians(angleY);
		float radZ = glm::radians(angleZ);

		// Create quaternion from Euler angles (in radians)
		glm::quat rotation = glm::quat(glm::vec3(radX, radY, radZ));

		// Update the node's rotation (w, x, y, z)
		node.rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
	}

	void initialize() {
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
		/*localTransforms.resize(model.nodes.size(), glm::mat4(1.0f));
		globalTransforms.resize(model.nodes.size(), glm::mat4(1.0f));
		for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
			computeLocalNodeTransform(model, (int)nodeIndex, localTransforms);
		}

		for (auto rootNodeIndex : scene.nodes) {
			computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, glm::mat4(1.0f), globalTransforms);
		}*/
		localTransforms.resize(model.nodes.size(), glm::mat4(1.0f));
		globalTransforms.resize(model.nodes.size(), glm::mat4(1.0f));

		//const tinygltf::Scene& scene = model.scenes[model.defaultScene];
		for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); nodeIndex++) {
			computeLocalNodeTransform(model, (int)nodeIndex, localTransforms);
			// 
			//computeLocalNodeTransform(model, 0, localTransforms);
			//computeLocalNodeTransform(model, 1, localTransforms);
		}

		for (auto rootNodeIndex : scene.nodes) {
			computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, glm::mat4(1.0f), globalTransforms);
			// 
			//computeGlobalNodeTransform(model, localTransforms, 0, glm::mat4(1.0f), globalTransforms);
			//computeGlobalNodeTransform(model, localTransforms, 1, glm::mat4(1.0f), globalTransforms);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
	
		// Prepare buffers for rendering 
		primitiveObjects = bindModel(model);

		// Prepare joint matrices
		//skinObjects = prepareSkinning(model);

		// Prepare animation data 
		animationObjects = prepareAnimation(model);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/shader/bot.vert", 
			"C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/lab__4/lab4/lab4/lab4/shader/bot.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for GLSL variables
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
	}
	// forward-declare
	//std::vector<PrimitiveObject> bindMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh);

	void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {

		std::map<int, GLuint> vbos;
		for (size_t i = 0; i < model.bufferViews.size(); ++i) {
			const tinygltf::BufferView &bufferView = model.bufferViews[i];

			int target = bufferView.target;
			
			if (bufferView.target == 0) { 
				// The bufferView with target == 0 in our model refers to 
				// the skinning weights, for 25 joints, each 4x4 matrix (16 floats), totaling to 400 floats or 1600 bytes. 
				// So it is considered safe to skip the warning.
				//std::cout << "WARN: bufferView.target is zero" << std::endl;
				continue;
			}

			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, bufferView.byteLength,
						&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
			
			vbos[i] = vbo;
		}

		// Each mesh can contain several primitives (or parts), each we need to 
		// bind to an OpenGL vertex array object
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {

			tinygltf::Primitive primitive = mesh.primitives[i];

			//tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			//tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			//for (auto& attrib : primitive.attributes) {
			//	tinygltf::Accessor accessor = model.accessors[attrib.second];
			//	int byteStride =
			//		accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			//	glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);
			//
			//	int size = 1;
			//	if (accessor.type != TINYGLTF_TYPE_SCALAR) {
			//		size = accessor.type;
			//	}
			//
			//	int vaa = -1;
			//	if (attrib.first.compare("POSITION") == 0) vaa = 0;
			//	if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			//	if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			//	if (attrib.first.compare("JOINTS_0") == 0) vaa = 3;
			//	if (attrib.first.compare("WEIGHTS_0") == 0) vaa = 4;
			//	if (vaa > -1) {
			//		glEnableVertexAttribArray(vaa);
			//		glVertexAttribPointer(vaa, size, accessor.componentType,
			//			accessor.normalized ? GL_TRUE : GL_FALSE,
			//			byteStride, BUFFER_OFFSET(accessor.byteOffset));
			//	}
			//	else {
			//		std::cout << "vaa missing: " << attrib.first << std::endl;
			//	}
			//
			//
			//}

			PrimitiveObject primitiveObject;

			glGenVertexArrays(1, &primitiveObject.vao);
			glBindVertexArray(primitiveObject.vao);







			// Record VAO for later use
			//primitiveObject.vao = vao;
			
			
			
			



			
			//GLuint index;
			tinygltf::Accessor indexAccessor = model.accessors[primitive.attributes["INDEX"]];
			//const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];

			glGenBuffers(1, &primitiveObject.index);
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.index);
			glBufferData(GL_ARRAY_BUFFER, indexBufferView.byteLength, &model.buffers[indexBufferView.buffer].data.at(0) + indexBufferView.byteOffset, GL_STATIC_DRAW);

			if (primitive.indices >= 0) {
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[indexBufferView.buffer];

				GLuint indexBuffer;
				glGenBuffers(1, &indexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength,
					buffer.data.data() + indexBufferView.byteOffset, GL_STATIC_DRAW);
				primitiveObject.index = indexBuffer;
			}







			tinygltf::Accessor normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
			tinygltf::BufferView normalBufferView = model.bufferViews[normalAccessor.bufferView];

			glGenBuffers(1, &primitiveObject.normal);
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.normal);
			glBufferData(GL_ARRAY_BUFFER, normalBufferView.byteLength, &model.buffers[normalBufferView.buffer].data.at(0) + normalBufferView.byteOffset, GL_STATIC_DRAW);






			tinygltf::Accessor positionAccessor = model.accessors[primitive.attributes["POSITION"]];
			tinygltf::BufferView positionBufferView = model.bufferViews[positionAccessor.bufferView];

			glGenBuffers(1, &primitiveObject.position);
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.position);
			glBufferData(GL_ARRAY_BUFFER, positionBufferView.byteLength, &model.buffers[positionBufferView.buffer].data.at(0) + positionBufferView.byteOffset, GL_STATIC_DRAW);

			
			tinygltf::Accessor textureAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
			tinygltf::BufferView textureBufferView = model.bufferViews[textureAccessor.bufferView];

			glGenBuffers(1, &primitiveObject.texture);
			glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.texture);
			glBufferData(GL_ARRAY_BUFFER, textureBufferView.byteLength, &model.buffers[textureBufferView.buffer].data.at(0) + textureBufferView.byteOffset, GL_STATIC_DRAW);

			
			
			
			
			
			primitiveObject.vbos = vbos;







			//if (model.textures.size() > 0) {
			//	// fixme: Use material's baseColor
			//	//int materialIndex = primitive.material;
			//	//const tinygltf::Material& mat = model.materials[materialIndex];
			//	tinygltf::Texture& tex = model.textures[1];

			//	if (tex.source > -1) {

			//		GLuint texid;
			//		glGenTextures(1, &texid);

			//		tinygltf::Image& image = model.images[tex.source];

			//		glBindTexture(GL_TEXTURE_2D, texid);
			//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//		GLenum format = GL_RGBA;

			//		if (image.component == 1) {
			//			format = GL_RED;
			//		}
			//		else if (image.component == 2) {
			//			format = GL_RG;
			//		}
			//		else if (image.component == 3) {
			//			format = GL_RGB;
			//		}
			//		else {
			//			// ???
			//		}

			//		GLenum type = GL_UNSIGNED_BYTE;
			//		if (image.bits == 8) {
			//			// ok
			//		}
			//		else if (image.bits == 16) {
			//			type = GL_UNSIGNED_SHORT;
			//		}
			//		else {
			//			// ???
			//		}

			//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
			//			format, type, &image.image.at(0));

			//		primitiveObject.textureID = texid;
			//	}


			//primitiveObjects.push_back(primitiveObject);
			//}

			int matIndex = primitive.material;                 // e.g. 0,1,2,3, ...
			if (matIndex >= 0 && matIndex < model.materials.size()) {
				tinygltf::Material& mat = model.materials[matIndex];

				// Example: get the baseColor texture from the PBR info
				if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
					int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
					if (textureIndex >= 0 && textureIndex < model.textures.size()) {
						tinygltf::Texture& tex = model.textures[textureIndex];
						if (tex.source >= 0 && tex.source < model.images.size()) {
							// Now you have the correct image index
							tinygltf::Image& image = model.images[tex.source];

							// Generate and bind a GL texture for this image
							GLuint texid;
							glGenTextures(1, &texid);
							glBindTexture(GL_TEXTURE_2D, texid);
							glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

							// Decide proper GL format
							GLenum format = GL_RGBA;
							if (image.component == 1) format = GL_RED;
							else if (image.component == 2) format = GL_RG;
							else if (image.component == 3) format = GL_RGB;
							// etc.

							GLenum type = GL_UNSIGNED_BYTE;
							if (image.bits == 16) type = GL_UNSIGNED_SHORT;

							// Upload the pixel data
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
								image.width, image.height, 0,
								format, type, &image.image.at(0));
							glGenerateMipmap(GL_TEXTURE_2D);

							// Store into your PrimitiveObject
							primitiveObject.textureID = texid;
						}
					}
				}
				else {
					// No baseColorTexture? Then handle default or skip
					primitiveObject.textureID = 0; // or a fallback
				}
			}
			else {
				// No material assigned
				primitiveObject.textureID = 0; // or a fallback
			 
			}
			primitiveObjects.push_back(primitiveObject);





			glBindVertexArray(0);
		}
		//}

	}


//	void bindMesh(std::map<int, GLuint>& vbos,
//              tinygltf::Model &model, tinygltf::Mesh &mesh) {
//  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
//    const tinygltf::BufferView &bufferView = model.bufferViews[i];
//    if (bufferView.target == 0) {  // TODO impl drawarrays
//      std::cout << "WARN: bufferView.target is zero" << std::endl;
//      continue;  // Unsupported bufferView.
//                 /*
//                   From spec2.0 readme:
//                   https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
//                            ... drawArrays function should be used with a count equal to
//                   the count            property of any of the accessors referenced by the
//                   attributes            property            (they are all equal for a given
//                   primitive).
//                 */
//    }
//
//    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
//    std::cout << "bufferview.target " << bufferView.target << std::endl;
//
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    vbos[i] = vbo;
//    glBindBuffer(bufferView.target, vbo);
//
//    std::cout << "buffer.data.size = " << buffer.data.size()
//              << ", bufferview.byteOffset = " << bufferView.byteOffset
//              << std::endl;
//
//    glBufferData(bufferView.target, bufferView.byteLength,
//                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
//  }
//
//  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
//    tinygltf::Primitive primitive = mesh.primitives[i];
//    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
//
//    for (auto &attrib : primitive.attributes) {
//      tinygltf::Accessor accessor = model.accessors[attrib.second];
//      int byteStride =
//          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
//      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);
//
//      int size = 1;
//      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
//        size = accessor.type;
//      }
//
//      int vaa = -1;
//      if (attrib.first.compare("POSITION") == 0) vaa = 0;
//      if (attrib.first.compare("NORMAL") == 0) vaa = 1;
//      if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
//      if (vaa > -1) {
//        glEnableVertexAttribArray(vaa);
//        glVertexAttribPointer(vaa, size, accessor.componentType,
//                              accessor.normalized ? GL_TRUE : GL_FALSE,
//                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
//      } else
//        std::cout << "vaa missing: " << attrib.first << std::endl;
//    }
//
//    if (model.textures.size() > 0) {
//      // fixme: Use material's baseColor
//      tinygltf::Texture &tex = model.textures[0];
//
//      if (tex.source > -1) {
//
//        GLuint texid;
//        glGenTextures(1, &texid);
//
//        tinygltf::Image &image = model.images[tex.source];
//
//        glBindTexture(GL_TEXTURE_2D, texid);
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//        GLenum format = GL_RGBA;
//
//        if (image.component == 1) {
//          format = GL_RED;
//        } else if (image.component == 2) {
//          format = GL_RG;
//        } else if (image.component == 3) {
//          format = GL_RGB;
//        } else {
//          // ???
//        }
//
//        GLenum type = GL_UNSIGNED_BYTE;
//        if (image.bits == 8) {
//          // ok
//        } else if (image.bits == 16) {
//          type = GL_UNSIGNED_SHORT;
//        } else {
//          // ???
//        }
//
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
//                     format, type, &image.image.at(0));
//      }
//    }
//  }
//}

// bind models
//void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model,
//                    tinygltf::Node &node) {
//  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
//    bindMesh(vbos, model, model.meshes[node.mesh]);
//  }
//
//  for (size_t i = 0; i < node.children.size(); i++) {
//    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
//    bindModelNodes(vbos, model, model.nodes[node.children[i]]);
//  }
//}



	void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, 
						tinygltf::Model &model,
						tinygltf::Node &node) {
		// Bind buffers for the current mesh at the node
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}

		// Recursive into children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}

	std::vector<PrimitiveObject> bindModel(tinygltf::Model &model) {
		std::vector<PrimitiveObject> primitiveObjects;

		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}

		return primitiveObjects;
	}
	
	//void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
	//			tinygltf::Model &model, tinygltf::Mesh &mesh) {
	//	
	//	for (size_t i = 0; i < mesh.primitives.size(); ++i) 
	//	{
	//		GLuint vao = primitiveObjects[i].vao;
	//		std::map<int, GLuint> vbos = primitiveObjects[i].vbos;
	//
	//		glBindVertexArray(vao);
	//
	//		tinygltf::Primitive primitive = mesh.primitives[i];
	//		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
	//
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
	//
	//		glDrawElements(primitive.mode, indexAccessor.count,
	//					indexAccessor.componentType,
	//					BUFFER_OFFSET(indexAccessor.byteOffset));
	//
	//		glBindVertexArray(0);
	//	}
	//}


	void drawMesh2(const std::vector<PrimitiveObject>& primitiveObjects, int primitiveIndex,
		tinygltf::Model& model, tinygltf::Mesh& mesh) {
		// Retrieve the specific PrimitiveObject to use for rendering
		const PrimitiveObject& primitiveObject = primitiveObjects[primitiveIndex];

		// Bind the VAO for the primitive
		glBindVertexArray(primitiveObject.vao);

		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			tinygltf::Primitive primitive = mesh.primitives[i];

			// Access the indices accessor from the model
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			// Bind the specific VBO for this primitive's index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitiveObject.index);

			// If the primitive uses a position buffer, bind it
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.position);
				// Enable and set vertex attribute pointer for position
				glEnableVertexAttribArray(0); // Assuming position is bound to location 0
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			// If the primitive uses a normal buffer, bind it
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.normal);
				// Enable and set vertex attribute pointer for normal
				glEnableVertexAttribArray(1); // Assuming normal is bound to location 1
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			// If the primitive uses a texture buffer, bind it
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.texture);
				// Enable and set vertex attribute pointer for texture coordinates
				glEnableVertexAttribArray(2); // Assuming texture coordinates are bound to location 2
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}

			// Draw the elements
			glDrawElements(primitive.mode, indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));

			// Disable the enabled attributes to clean up
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

		// Unbind the VAO after rendering
		glBindVertexArray(0);
	}

	void drawMesh3(const std::vector<PrimitiveObject>& primitiveObjects, int primitiveIndex,
		tinygltf::Model& model, tinygltf::Mesh& mesh) {
		const PrimitiveObject& primitiveObject = primitiveObjects[primitiveIndex];

		if (!glIsVertexArray(primitiveObject.vao)) {
			std::cerr << "Error: Invalid VAO for primitive index " << primitiveIndex << std::endl;
			return;
		}

		glBindVertexArray(primitiveObject.vao);

		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			if (primitive.indices < 0) {
				std::cerr << "Error: Missing indices for primitive " << i << std::endl;
				continue;
			}

			const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			if (!glIsBuffer(primitiveObject.index)) {
				std::cerr << "Error: Invalid index buffer for primitive " << i << std::endl;
				continue;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitiveObject.index);

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

			glDrawElements(primitive.mode, indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

		glBindVertexArray(0);
	}

	void drawMesh(const std::vector<PrimitiveObject>& primitiveObjects, int primitiveIndex,
		tinygltf::Model& model, tinygltf::Mesh& mesh) {

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

			// Bind vertex attributes
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.position);
				glEnableVertexAttribArray(0); // Assuming position is at location 0
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.normal);
				glEnableVertexAttribArray(1); // Assuming normal is at location 1
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				glBindBuffer(GL_ARRAY_BUFFER, primitiveObject.texture);
				glEnableVertexAttribArray(2); // Assuming texcoord is at location 2
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}

			// Check if the primitive has indices
			if (primitive.indices >= 0) {
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitiveObject.index);

				// Draw using indices
				glDrawElements(primitive.mode, indexAccessor.count,
					indexAccessor.componentType,
					BUFFER_OFFSET(indexAccessor.byteOffset));
			}
			else {
				// No indices; draw using glDrawArrays

				const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
				glDrawArrays(primitive.mode, 0, positionAccessor.count);
			}

			// Disable vertex attributes after drawing
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

		glBindVertexArray(0);
	}



	void drawMesh1(const std::vector<PrimitiveObject>& primitiveObjects, int primitiveIndex,
		tinygltf::Model& model, tinygltf::Mesh& mesh) {
		GLuint vao = primitiveObjects[primitiveIndex].vao;
		std::map<int, GLuint> vbos = primitiveObjects[primitiveIndex].vbos;

		glBindVertexArray(vao);

		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

			glDrawElements(primitive.mode, indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));
		}

		glBindVertexArray(0);
	}

	void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, 
		tinygltf::Node& node, const glm::mat4& vp, const glm::mat4& parentMatrix, int nodeIndex) {
		
		///////////////////////////////////////////////////////////////////////////
		// Compute the model matrix for the current node
		glm::mat4 modelMatrix = parentMatrix * globalTransforms[nodeIndex];

		// Compute the MVP matrix
		glm::mat4 mvp = vp * modelMatrix;

		// Pass the MVP to the shader
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, glm::value_ptr(mvp));

		///////////////////////////////////////////////////////////////////////////
		// Draw the mesh at the node, if available
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			tinygltf::Mesh& mesh = model.meshes[node.mesh];

			// Loop through all primitives in the mesh
			for (size_t i = 0; i < mesh.primitives.size(); ++i) {
				//drawMesh(primitiveObjects, node.mesh + i, model, mesh);  // Pass the primitive index
				drawMesh(primitiveObjects, node.mesh, model, mesh);  // Pass the primitive index
			}
		}

		// Recursively draw children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]], vp, modelMatrix, i);
		}
	}

	void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
				tinygltf::Model &model, const glm::mat4 &vp) {
		// Draw all nodes
		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]], vp, glm::mat4(1.0f), i);
		}
	}

	//void render(glm::mat4 cameraMatrix) {
	void render(glm::mat4 cameraMatrix, const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix) {

		glUseProgram(programID);
		//
		//// Set camera
		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		//// -----------------------------------------------------------------
		//// TODO: Set animation data for linear blend skinning in shader
		//// -----------------------------------------------------------------
		//// -----------------------------------------------------------------

		//// Set light data 
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);


		//glActiveTexture(GL_TEXTURE0); // Use texture unit 0
		//GLuint texid;
		//glGenTextures(1, &texid); 
		//glBindTexture(GL_TEXTURE_2D, texid); // Bind the loaded texture
		//glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0); // Set sampler to use texture unit 0

		//for (const auto& primitive : primitiveObjects) {
		//	glActiveTexture(GL_TEXTURE0); // Activate texture unit
		//	glBindTexture(GL_TEXTURE_2D, primitive.textureID); // Bind texture
		//	glDrawElements(GL_TRIANGLES, primitive.indicesCount, GL_UNSIGNED_INT, 0); // Draw the primitive
		//}
		for (auto& pObj : primitiveObjects) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pObj.textureID);
			glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
		}

		//// Draw the GLTF model
		drawModel(primitiveObjects, model, projectionMatrix * viewMatrix);


		//glm::mat4 modelMatrix = glm::mat4(1.0f);  // Default for now, update if needed
		//glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		//glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		//glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}


	void cleanup() {
		glDeleteProgram(programID);
	}
}; 















//int main(void)
//{
//	// Initialise GLFW
//	if (!glfwInit())
//	{
//		std::cerr << "Failed to initialize GLFW." << std::endl;
//		return -1;
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	// Open a window and create its OpenGL context
//	window = glfwCreateWindow(windowWidth, windowHeight, "Lab 4", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cerr << "Failed to open a GLFW window." << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	// Ensure we can capture the escape key being pressed below
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//	glfwSetKeyCallback(window, key_callback);
//
//	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
//	int version = gladLoadGL(glfwGetProcAddress);
//	if (version == 0)
//	{
//		std::cerr << "Failed to initialize OpenGL context." << std::endl;
//		return -1;
//	}
//
//	// Background
//	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
//
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//
//	// Our 3D character
//	MyBot bot;
//	bot.initialize();
//
//	// Camera setup
//    glm::mat4 viewMatrix, projectionMatrix;
//	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);
//
//	// Time and frame rate tracking
//	static double lastTime = glfwGetTime();
//	float time = 0.0f;			// Animation time 
//	float fTime = 0.0f;			// Time for measuring fps
//	unsigned long frames = 0;
//
//	// Main loop
//	do
//	{
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Update states for animation
//        double currentTime = glfwGetTime();
//        float deltaTime = float(currentTime - lastTime);
//		lastTime = currentTime;
//
//		if (playAnimation) {
//			time += deltaTime * playbackSpeed;
//			bot.update(time);
//		}
//
//		// Rendering
//		viewMatrix = glm::lookAt(eye_center, lookat, up);
//		glm::mat4 vp = projectionMatrix * viewMatrix;
//		//bot.render(vp);
//		bot.render(vp, projectionMatrix, viewMatrix);
//
//
//		// FPS tracking 
//		// Count number of frames over a few seconds and take average
//		frames++;
//		fTime += deltaTime;
//		if (fTime > 2.0f) {		
//			float fps = frames / fTime;
//			frames = 0;
//			fTime = 0;
//			
//			std::stringstream stream;
//			stream << std::fixed << std::setprecision(2) << "Lab 4 | Frames per second (FPS): " << fps;
//			glfwSetWindowTitle(window, stream.str().c_str());
//		}
//
//		// Swap buffers
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//	} // Check if the ESC key was pressed or the window was closed
//	while (!glfwWindowShouldClose(window));
//
//	// Clean up
//	bot.cleanup();
//
//	// Close OpenGL window and terminate GLFW
//	glfwTerminate();
//
//	return 0;
//}
//













//static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
//{
//	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
//	{
//		playbackSpeed += 1.0f;
//		if (playbackSpeed > 10.0f) 
//			playbackSpeed = 10.0f;
//	}
//
//	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
//	{
//		playbackSpeed -= 1.0f;
//		if (playbackSpeed < 1.0f) {
//			playbackSpeed = 1.0f;
//		}
//	}
//
//	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
//		playAnimation = !playAnimation;
//	}
//
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}





//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
//	glm::vec3 forward = glm::normalize(lookat - eye_center);
//
//	// Calculate right (perpendicular direction to both 'up' and 'forward')
//	glm::vec3 right = glm::normalize(glm::cross(up, forward));
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//		// Move forward (along the 'lookat' direction)
//		eye_center += cameraSpeed * forward;
//		lookat += cameraSpeed * forward;
//	}
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//		// Move left (perpendicular to the 'lookat' direction)
//		eye_center += cameraSpeed * right;
//		lookat += cameraSpeed * right;
//	}
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//		// Move backward (opposite of the 'lookat' direction)
//		eye_center -= cameraSpeed * forward;
//		lookat -= cameraSpeed * forward;
//	}
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//		// Move right (perpendicular to the 'lookat' direction)
//		eye_center -= cameraSpeed * right;
//		lookat -= cameraSpeed * right;
//	}
//	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
//	{
//		playbackSpeed += 1.0f;
//		if (playbackSpeed > 10.0f)
//			playbackSpeed = 10.0f;
//	}
//
//	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
//	{
//		playbackSpeed -= 1.0f;
//		if (playbackSpeed < 1.0f) {
//			playbackSpeed = 1.0f;
//		}
//	}
//
//	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
//		playAnimation = !playAnimation;
//	}
//
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}

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
		playAnimation = !playAnimation;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}