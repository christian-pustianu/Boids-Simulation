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
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
}

namespace
{
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 720;

    constexpr float kPi_ = 3.1415926f;

    constexpr float kMovementPerSecond_ = 5.f; // units per second
    constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

    struct CameraState
    {
        bool cameraActive;
        bool actionZoomIn, actionZoomOut;

        float phi, theta;
        float radius;

        float lastX, lastY;
    };

    void glfw_callback_error_(int, char const*);

    void glfw_callback_key_(GLFWwindow*, int, int, int, int);
    void glfw_callback_motion_(GLFWwindow*, double, double);
}


int main()
{
    if (!glfwInit())
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
        // Set up event handling
    CameraState camera{};

    glfwSetWindowUserPointer(window, &camera);

    glfwSetKeyCallback(window, &glfw_callback_key_);
    glfwSetCursorPosCallback(window, &glfw_callback_motion_);

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


    camera.radius = 10.f;
    // Animation state
    auto last = std::chrono::steady_clock::now();

    // Define objects
    Render object = Render(make_cone());

    // Start the rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Update state
        auto const now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - last).count();
        last = now;

        // Update camera state
        if (camera.actionZoomIn)
            camera.radius -= kMovementPerSecond_ * dt;
        else if (camera.actionZoomOut)
            camera.radius += kMovementPerSecond_ * dt;

        if (camera.radius <= 0.1f)
            camera.radius = 0.1f;

        // Set a clear color.
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

        Mat44f model2world = make_rotation_y(0);

        Mat44f Rx = make_rotation_x(camera.theta);
        Mat44f Ry = make_rotation_y(camera.phi);
        Mat44f T = make_translation({ 0.f, 0.f, -camera.radius });
        Mat44f world2camera = T * Rx * Ry;

        //Mat44f world2camera = make_translation({ 0.f, 0.f, -3.f});

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

        glUniformMatrix4fv(
            1,
            1, GL_TRUE, model2world.v
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


// Setup for GLFW callbacks
namespace
{
    void glfw_callback_error_(int aErrNum, char const* aErrDesc)
    {
        std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
    }

    void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
    {
        if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
        {
            glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
            return;
        }

        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(aWindow)))
        {

            // Space toggles camera
            if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
            {
                camera->cameraActive = !camera->cameraActive;

                if (camera->cameraActive)
                    glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                else
                    glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // Camera controls if camera is active
            if (camera->cameraActive)
            {
                if (GLFW_KEY_W == aKey)
                {
                    if (GLFW_PRESS == aAction)
                        camera->actionZoomIn = true;
                    else if (GLFW_RELEASE == aAction)
                        camera->actionZoomIn = false;
                }
                else if (GLFW_KEY_S == aKey)
                {
                    if (GLFW_PRESS == aAction)
                        camera->actionZoomOut = true;
                    else if (GLFW_RELEASE == aAction)
                        camera->actionZoomOut = false;
                }
            }
        }
    }

    void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
    {
        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(aWindow)))
        {
            if (camera->cameraActive)
            {
                auto const dx = float(aX - camera->lastX);
                auto const dy = float(aY - camera->lastY);

                camera->phi += dx * kMouseSensitivity_;

                camera->theta += dy * kMouseSensitivity_;
                if (camera->theta > kPi_ / 2.f)
                    camera->theta = kPi_ / 2.f;
                else if (camera->theta < -kPi_ / 2.f)
                    camera->theta = -kPi_ / 2.f;
            }

            camera->lastX = float(aX);
            camera->lastY = float(aY);
        }
    }
}