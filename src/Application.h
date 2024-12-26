#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h> // For window and input handling

class Application {
public:
    // Constructor
    Application();

    // Destructor
    ~Application();

    // Initialize the application
    bool Init();

    // Run the main application loop
    void Run();

    // Shutdown and cleanup
    void Shutdown();

private:
    // GLFW window
    GLFWwindow* window;

    // Screen dimensions
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Initialize the GLFW window
    bool InitWindow();

    // Render the scene
    void Render();

    // Update logic
    void Update(float deltaTime);

    // Handle input
    void HandleInput();
};

#endif // APPLICATION_H
