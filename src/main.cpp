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

#include "skybox.h"
#include "plane.h"
#include "loader.h" // Include MyBot class
#include "render/shader.h" // Include LoadShaders function
//////////////////////////////////////////////////////////////////////////////////////////////




static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


static GLFWwindow* window;
static int windowWidth = 1024;
static int windowHeight = 768;

// Camera
 static float cameraSpeed = 1;
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



std::vector<std::string> faces = {
	"textures/posX.png", // Positive X
	"textures/negX.png", // Negative X
	"textures/posY.png", // Positive Y
	"textures/negY.png", // Negative Y
	"textures/posZ.png", // Positive Z
	"textures/negZ.png"  // Negative Z
};














int main() {
    // Initialize GLFW and GLAD
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);







	// Instantiate the Plane object
	Plane plane;

	// Inside main() after MyBot initialization
	plane.initialize();




	//Skybox skybox(faces);
	//skybox.initialize();

	Skybox skybox("C:/MyStuff/Mymy_Old/newDocs/ICS_24_25/COMPUTER_GRAPHICS/final_project/emerald/Toward_a_Futuristic_Emerald_Isle/src/model/sky.png"); // Single image for skybox
	skybox.initialize();





    // Load and initialize the model
    MyBot bot;
    bot.initialize();

	glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time 
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;





        // Update the view matrix
        viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;



		//bot.render(vp);
		bot.render(vp, projectionMatrix, viewMatrix);




		skybox.render(viewMatrix, projectionMatrix);



		//glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix for the plane
		//glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)), glm::vec3(50.0f, 1.0f, 50.0f));
		glm::mat4 modelMatrix = glm::scale(
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
			glm::vec3(0.01f, 1.0f, 0.01f) // Tiny plane
		);
		plane.render(vp, modelMatrix);
		glEnable(GL_DEPTH_TEST);


		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Lab 4 | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	plane.cleanup();
    bot.cleanup();
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
}