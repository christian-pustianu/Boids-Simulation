#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include "Shader.h"
#include "Render.h"
#include "Mesh.h"
#include "cone.hpp"

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
    if(!glfwInit())
    {
		throw std::exception("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "Boids-Simulation",
        nullptr, nullptr);
    
    if (window == NULL)
    {
        glfwTerminate();
        throw std::exception("Failed to create GLFW window");
    }

    // Set up event handling (instead of processInput())
    //glfwSetKeyCallback(window, &glfw_callback_key_);
    //glfwSetCursorPosCallback(window, &glfw_callback_motion_);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::exception("Failed to load GLAD");
    }

    std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
    std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
    std::printf("VERSION %s\n", glGetString(GL_VERSION));
    std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
	// Set viewport to current window size
    int iwidth, iheight;
    glfwGetFramebufferSize(window, &iwidth, &iheight);
    glViewport(0, 0, iwidth, iheight);

    // Set up shaders
    Shader shader = Shader("assets/default.vert", "assets/default.frag");

    // Define objects
    Render object = Render(make_cone());

    // Start the rendering loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Set a clear color.
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

        //Mat44f model2world = make_rotation_y(0);

        //Mat44f Rx = make_rotation_x(state.camControl.theta);
        //Mat44f Ry = make_rotation_y(state.camControl.phi);
        //Mat44f T = make_translation({ 0.f, 0.f, -state.camControl.radius });
        //Mat44f world2camera = T * Rx * Ry;

        Mat44f model2world = make_rotation_y(19);
        Mat44f world2camera = make_translation({ 0.f, 0.f, -3.f});

        Mat44f projection = make_perspective_projection(
            60.f * 3.1415926f / 180.f, // fov = 60 degrees
            iwidth / float(iheight),
            0.1f, 100.0f
        );

        Mat44f projCameraWorld = projection * world2camera * model2world;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(
            0,
            1, GL_TRUE, projCameraWorld.v
        );

        // Render objects with specified shader
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        object.render(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean-up
    shader.~Shader();
    object.~Render();

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


// Setup for GLFW callbacks
// Initialise at the beginning of the file too!
// void glfw_callback_error_(int aErrNum, char const* aErrDesc)
// void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int mods)
// void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)