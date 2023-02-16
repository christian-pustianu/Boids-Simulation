#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>

#include "Shader.h"
#include "Render.h"
#include "Mesh.h"
#include "cone.hpp"

extern "C"
{
    // Force the use of the NVIDIA GPU on laptops with switchable graphics
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
}

namespace
{
    // Global variables
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 720;

    constexpr float PI = 3.1415926f;

    constexpr float CAMERA_MOVEMENT = 10.f;
    constexpr float MOUSE_SENSITIVITY = 0.01f;

    struct CameraState
    {
        bool active;
        bool moveForward, moveBackwards;

        float xRotation, yRotation;
        float zTranslation;

        float lastXPos, lastYPos;
    };

    // Callaback definitions for glfw
    void error_callback(int, const char*);
    void key_callback(GLFWwindow*, int, int, int, int);
    void cursor_position_callback(GLFWwindow*, double, double);
}


int main()
{
    // Initialize glfw
    if (!glfwInit())
    {
        throw std::exception("Failed to initialize GLFW");
    }

    // Set up glfw error handling
    glfwSetErrorCallback(&error_callback);

    // Set up glfw window hints
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "Boids-Simulation",
        nullptr, nullptr);

    if (window == NULL)
    {
        glfwTerminate();
        throw std::exception("Failed to create GLFW window");
    }

    // Initialize camera
    CameraState camera{};
    camera.zTranslation = 5.f;

    // Set up glfw event handling
    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, &key_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);

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

    // Set a clear color
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    
    // Set viewport to current window size
    int iwidth, iheight;
    glfwGetFramebufferSize(window, &iwidth, &iheight);
    glViewport(0, 0, iwidth, iheight);

    // Initialize time for animations
    auto last = std::chrono::steady_clock::now();
    float angle = 0.f;
    
    // Set up shaders
    Shader shader = Shader("assets/default.vert", "assets/default.frag");

    // Define objects
    Render object = Render(make_cone());

    // Start the rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Update time dt between frames
        auto const now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - last).count();
        last = now;

        // Update camera state
        if (camera.moveForward)
            camera.zTranslation -= CAMERA_MOVEMENT * dt;
        else if (camera.moveBackwards)
            camera.zTranslation += CAMERA_MOVEMENT * dt;

        if (camera.zTranslation <= 0.1f)
            camera.zTranslation = 0.1f;

        angle += 0.5f * dt;

        // Set up projection to screen
        Mat44f model2world = make_rotation_z(angle);

        Mat44f Rx = make_rotation_x(camera.yRotation);
        Mat44f Ry = make_rotation_y(camera.xRotation);
        Mat44f T = make_translation({ 0.f, 0.f, -camera.zTranslation });
        Mat44f world2camera = T * Rx * Ry;

        Mat44f projection = make_perspective_projection(
            60.f * 3.1415926f / 180.f, // fov = 60 degrees
            iwidth / float(iheight),
            0.1f, 100.0f
        );

        Mat44f world2projection = projection * world2camera;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(
            0,
            1, GL_TRUE, world2projection.v
        );

        glUniformMatrix4fv(
            1,
            1, GL_TRUE, model2world.v
        );

        // Camera
        Vec3f cameraPosition = { 0.f, 0.f, camera.zTranslation };
        glUniform3f(2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        // Enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Draw blended objects

        glDisable(GL_BLEND);
        
        // Render objects with specified shader
        object.render(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean-up
    object.~Render();
    shader.~Shader();

    glfwTerminate();
    return 0;
}


// Setup for GLFW callbacks
namespace
{
    void error_callback(int code, const char* description)
    {
        std::fprintf(stderr, "GLFW error: %s (%d)\n", description, code);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window)))
        {

            // Space toggles camera
            if (GLFW_KEY_SPACE == key && GLFW_PRESS == action)
            {
                camera->active = !camera->active;

                if (camera->active)
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                else
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // Camera controls if camera is active
            if (camera->active)
            {
                if (GLFW_KEY_W == key)
                {
                    if (GLFW_PRESS == action)
                        camera->moveForward = true;
                    else if (GLFW_RELEASE == action)
                        camera->moveForward = false;
                }
                else if (GLFW_KEY_S == key)
                {
                    if (GLFW_PRESS == action)
                        camera->moveBackwards = true;
                    else if (GLFW_RELEASE == action)
                        camera->moveBackwards = false;
                }
            }
        }
    }

    void cursor_position_callback(GLFWwindow* window, double xPos, double yPos)
    {
        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window)))
        {
            if (camera->active)
            {
                auto const dx = float(xPos - camera->lastXPos);
                auto const dy = float(yPos - camera->lastYPos);

                camera->xRotation += dx * MOUSE_SENSITIVITY;
                camera->yRotation += dy * MOUSE_SENSITIVITY;

                if (camera->yRotation > PI / 2.f)
                    camera->yRotation = PI / 2.f;
                else if (camera->yRotation < -PI / 2.f)
                    camera->yRotation = -PI / 2.f;
            }

            camera->lastXPos = float(xPos);
            camera->lastYPos = float(yPos);
        }
    }
}