#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include "Shader.h"
#include "Object.h"

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
}

namespace
{
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 720;
}

void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "Boids-Simulation",
        nullptr, nullptr);
    
    if (window == NULL)
    {
        std::printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::printf("Failed to initialize GLAD");
        return -1;
    }

    std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
    std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
    std::printf("VERSION %s\n", glGetString(GL_VERSION));
    std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    Shader shader = Shader("assets/default.vert", "assets/default.frag");

    Object object = Object();

    int iwidth, iheight;
    glfwGetFramebufferSize(window, &iwidth, &iheight);

    glViewport(0, 0, iwidth, iheight);

    // Start rendering infinitely.
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Render
        object.render(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean-up
    shader.data.cleanup();
    object.data.cleanup();

    glfwTerminate();
    return 0;
}

/** Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly. */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}