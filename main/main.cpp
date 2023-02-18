#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>

#include "Shader.hpp"
#include "Render.hpp"
#include "Mesh.hpp"
#include "cone.hpp"
#include "loadobj.hpp"
#include "../math/mat44.hpp"

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

    constexpr unsigned int ARC_BALL = 0;
    constexpr unsigned int FlY_THROUGH = 1;
    constexpr unsigned int TOP_DOWN = 2;
    constexpr unsigned int THIRD_PERSON = 3;

    struct CameraState
    {
        bool active;
        struct move { bool forward, backwards, left, right, up, down; } move;

        Vec2f rotation = { 0.f, 0.f };
        Vec3f position = { 0.f, 0.f, 0.f };
        unsigned int mode = ARC_BALL; // default mode

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
    camera.position.z = 5.f;

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    // Initialize time for animations
    auto last = std::chrono::steady_clock::now();
    float angle = 0.f;
    
    // Set up shaders
    Shader shader = Shader("assets/BlinnPhong.vert", "assets/BlinnPhong.frag");

    // Define objects
    //Render object = Render(load_wavefront_obj("assets/Armadillo.obj"));
    Render object = Render(make_cone(true, 16, {1.f, 1.f, 1.f}, make_scaling(3.f, 1.f, 1.f)));

    // Start the rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        // Set viewport to current window size
        int nwidth, nheight;
        glfwGetFramebufferSize(window, &nwidth, &nheight);

        // if the window is minimized, wait for it to be restored
        while (0 == nwidth || 0 == nheight)
        {
            glfwWaitEvents();
			glfwGetFramebufferSize(window, &nwidth, &nheight);
        }

        glViewport(0, 0, nwidth, nheight);

        // Update time dt between frames
        auto const now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - last).count();
        last = now;

        // Update camera state
        if (camera.mode == ARC_BALL)
        {
            if (camera.move.forward)
                camera.position.z -= CAMERA_MOVEMENT * dt;
            else if (camera.move.backwards)
                camera.position.z += CAMERA_MOVEMENT * dt;

            if (camera.position.z <= 0.1f)
                camera.position.z = 0.1f;
        }

        angle += 0.5f * dt;

        // Set up projection to screen
        Mat44f model2world = make_rotation_z(angle);

        Mat44f world2camera = kIdentity44f;
        if (camera.mode == ARC_BALL)
        {
            Mat44f Rx = make_rotation_x(camera.rotation.y);
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry;
        }
        else if (camera.mode == FlY_THROUGH)
        {
			Mat44f Rx = make_rotation_x(camera.rotation.y);
			Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
			world2camera = Rx * Ry * T;
		}
        else if (camera.mode == TOP_DOWN)
        {
            world2camera = kIdentity44f;
        }
        else if (camera.mode == THIRD_PERSON)
        {
            world2camera = kIdentity44f;
        }

        Mat44f projection = make_perspective_projection(
            60.f * 3.1415926f / 180.f, // fov = 60 degrees
            nwidth / float(nheight),
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
        glUniform3f(2, camera.position.x, camera.position.y, camera.position.z);

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
                        camera->move.forward = true;
                    else if (GLFW_RELEASE == action)
                        camera->move.forward = false;
                }
                else if (GLFW_KEY_S == key)
                {
                    if (GLFW_PRESS == action)
                        camera->move.backwards = true;
                    else if (GLFW_RELEASE == action)
                        camera->move.backwards = false;
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

                camera->rotation.x += dx * MOUSE_SENSITIVITY;
                camera->rotation.y += dy * MOUSE_SENSITIVITY;

                if (camera->rotation.y > PI / 2.f)
                    camera->rotation.y = PI / 2.f;
                else if (camera->rotation.y < -PI / 2.f)
                    camera->rotation.y = -PI / 2.f;
            }

            camera->lastXPos = float(xPos);
            camera->lastYPos = float(yPos);
        }
    }
}