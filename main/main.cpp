#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>

#include "Shader.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "Boid.hpp"
#include "Obstacle.hpp"

#include "cone.hpp"
#include "loadobj.hpp"

#include "../math/mat44.hpp"
#include "../math/other.hpp"

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

    constexpr float CAMERA_MOVEMENT = 50.f;
    constexpr float MOUSE_SENSITIVITY = 0.01f;

    constexpr unsigned int LOCKED_ARC_BALL = 1;
    constexpr unsigned int TOP_DOWN = 2;
    constexpr unsigned int FlY_THROUGH = 3;
    constexpr unsigned int THIRD_PERSON = 4;
    
    // Number of obstacles
    constexpr unsigned int NUM_SPHERES = 3;
    constexpr unsigned int NUM_BOXES = 3;
    constexpr unsigned int NUM_COLUMNS = 3;

    constexpr float SIMULATION_X = 100.f;
    constexpr float SIMULATION_Y = 50.f;
    constexpr float SIMULATION_Z = 100.f;

    constexpr float BOID_SPEED = 40.f;
    constexpr float BOID_VISION_RANGE = 12.f;

    unsigned int boids_count = 500;
    unsigned int boid_camera = 0;

    // Pause switch for the simulation
    bool paused = true;

    struct CameraState
    {
        bool active;
        struct move { bool forward, backwards, left, right, up, down; } move;

        Vec2f rotation = { 0.f, 0.f };
        Vec3f position = { 0.f, 0.f, 150.f };
        unsigned int mode = LOCKED_ARC_BALL; // default mode

        float yaw = -90.f;
        float pitch = 0.f;

        float lastXPos, lastYPos;
        Vec3f front = { 0.f, 0.f, -1.f };
        Vec3f up = { 0.f, 1.f, 0.f };
    };

    // Callaback definitions for glfw
    void error_callback(int, const char*);
    void key_callback(GLFWwindow*, int, int, int, int);
    void cursor_position_callback(GLFWwindow*, double, double);

    // Rules initialization for the boids
    Vec3f cohesion = { 0.f, 0.f, 0.f };
    Vec3f alignment = { 0.f, 0.f, 0.f };
    Vec3f separation = { 0.f, 0.f, 0.f };
    Vec3f avoid = { 0.f, 0.f, 0.f };
    Vec3f target_location = { 0.f, 0.f, 0.f };
    Vec3f target_direction = { 0.f, 0.f, 0.f };

    constexpr unsigned int NO_DIRECTION = 0;
    constexpr unsigned int DIRECTION_GIVEN = 1;
    constexpr unsigned int POINT_GIVEN = 2;

    unsigned int boid_control = POINT_GIVEN;
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
    
    // Set up shaders
    Shader shader = Shader("assets/BlinnPhong.vert", "assets/BlinnPhong.frag");

    // Define objects
    //Model armadillo = Model(load_wavefront_obj("assets/Armadillo.obj"));

    Model terrain = Model(load_wavefront_obj("assets/terrain.obj"));

    //Model column = Model(load_wavefront_obj("assets/column.obj"));

    Model box = Model(load_wavefront_obj("assets/box.obj"));

    Model sphere = Model(load_wavefront_obj("assets/sphere.obj"));

    std::vector<Obstacle*> obstacles;
    //obstacles.push_back(new SphereObstacle(&sphere, Vec3f{0.f, 0.f, 0.f}, 1.f));
    //obstacles.push_back(new SphereObstacle(&sphere, Vec3f{0.f, 25.f, 0.f}, 50.f));
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ SIMULATION_X, 25.f, SIMULATION_Z }, 10.f));
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ SIMULATION_X, 25.f, -SIMULATION_Z }, 10.f));
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ -SIMULATION_X, 25.f, SIMULATION_Z }, 10.f));
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ -SIMULATION_X, 25.f, -SIMULATION_Z }, 10.f));


    Model boid_model = Model(make_cone(true, 16, {1.f, 1.f, 1.f}, make_scaling(3.f, 1.f, 1.f)));

    std::vector<Boid*> boids;
    srand((time(NULL)));
    for (unsigned int i = 0; i < boids_count; i++)
    {
	    boids.push_back(new Boid());
    }

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

        // CAMERA MOVEMENT
        float measurmentUnit = CAMERA_MOVEMENT * dt;
        // Update camera state
        if (camera.mode == LOCKED_ARC_BALL || camera.mode == TOP_DOWN)
        {
            if (camera.move.forward)
                camera.position.z -= measurmentUnit;
            else if (camera.move.backwards)
                camera.position.z += measurmentUnit;

            if (camera.position.z <= 0.1f)
                camera.position.z = 0.1f;
        }
        else if (camera.mode == FlY_THROUGH)
        {
            if (camera.move.forward)
                camera.position += camera.front * measurmentUnit;
            else if (camera.move.backwards)
                camera.position -= camera.front * measurmentUnit;
            else if (camera.move.left)
                camera.position -= normalize(cross(camera.front, camera.up)) * measurmentUnit;
            else if (camera.move.right)
                camera.position += normalize(cross(camera.front, camera.up)) * measurmentUnit;
            else if (camera.move.up)
				camera.position += camera.up * measurmentUnit;
			else if (camera.move.down)
				camera.position -= camera.up * measurmentUnit;
        }

        // world2camera matrix for different camera modes
        Mat44f world2camera = Identity44f;
        if (camera.mode == LOCKED_ARC_BALL)
        {
            Mat44f Rx = make_rotation_x(radians(30.f));
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry;
        }
        else if (camera.mode == FlY_THROUGH)
        {
            world2camera = look_at(camera.position, camera.front, camera.up);
		}
        else if (camera.mode == TOP_DOWN)
        {
            Mat44f Rx = make_rotation_x(radians(90.f));
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry;
        }
        else if (camera.mode == THIRD_PERSON)
        {
            world2camera = look_at(boids.at(boid_camera)->currentDirection + Vec3f{0.f, 0.f, -5.f}, 
                boids.at(boid_camera)->currentPosition,
                {0.f, 1.f, 0.f});
        }


        // Set up projection to screen
        Mat44f projection = make_perspective_projection(
            60.f * 3.1415926f / 180.f, // fov = 60 degrees
            nwidth / float(nheight),
            0.1f, 100.0f
        );

        Mat44f world2projection = projection * world2camera;
        glUniformMatrix4fv(
            0,
            1, GL_TRUE, world2projection.v
        );


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera
        glUniform3f(2, camera.position.x, camera.position.y, camera.position.z);

        // Terrain position in world
        terrain.model2world = make_translation({ 0.f, -1.f, 0.f }) * make_scaling(SIMULATION_X, 0.f, SIMULATION_Z);

        // Simulation parameters
        float movement_speed = dt * BOID_SPEED;
        float turn_sharpness = movement_speed * 0.2f;

        //#pragma omp parallel for
        for (auto boid : boids) {
            if (!paused) {
                std::vector<Boid*> neighbours = boid->findNeighbours(boids, BOID_VISION_RANGE);
                cohesion = boid->applyCohesion(neighbours, 1.f);
                alignment = boid->applyAlignment(neighbours, 1.f);
                separation = boid->applySeparation(neighbours, 3.f, BOID_VISION_RANGE);
                avoid = boid->avoidEdges(2.f) + boid->avoidObstacles(obstacles, 3.f);

                if (boid_control == DIRECTION_GIVEN) {
                    target_direction = { 1.f, 0.f, 0.f};
                }
                else if (boid_control == POINT_GIVEN) {
                    target_location = { 0.f, 10.f, 0.f };
                    target_direction = normalize(target_direction - boid->currentPosition);
                }

                boid->setTargetDirection(normalize(boid->currentDirection + 
                    cohesion + alignment + separation + target_direction) + avoid);
                boid->updateDirection(movement_speed, turn_sharpness);
            }
            // Instanced rendering of boid_model for every boid created
            boid_model.render(boid->model2world, shader);
        }

        // Render terrain with specified shader
        terrain.render(terrain.model2world, shader);

        // Location point for directed movement
        sphere.render(make_translation(target_location), shader);
        
        // Enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw blended objects

        // Render obstacles
        for (auto obstacle : obstacles) {
            obstacle->model->render(obstacle->model2world, shader);
		}

        glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean-up
    terrain.~Model();
    boid_model.~Model();
    shader.~Shader();

    for (auto boid : boids) {
		delete boid;
	}

    for (auto obstacle : obstacles) {
		delete obstacle;
	}

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

        if (GLFW_KEY_SPACE == key && GLFW_PRESS == action)
        {
            paused = !paused;
        }

        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window)))
        {

            // Space toggles camera
            if (GLFW_KEY_C == key && GLFW_PRESS == action)
            {
                camera->active = !camera->active;

                if (camera->active)
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                else
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // Camera mode
            if (GLFW_KEY_1 == key && GLFW_PRESS == action)
				camera->mode = LOCKED_ARC_BALL;
			else if (GLFW_KEY_2 == key && GLFW_PRESS == action)
				camera->mode = TOP_DOWN;
            else if (GLFW_KEY_3 == key && GLFW_PRESS == action)
				camera->mode = FlY_THROUGH;
            else if (GLFW_KEY_4 == key && GLFW_PRESS == action) {
                boid_camera = rand() % boids_count;
				camera->mode = THIRD_PERSON;
            }

            // Camera controls if camera is active
            if (camera->active)
            {
                // Movement controls
                if (GLFW_KEY_A == key)
                {
					if (GLFW_PRESS == action)
						camera->move.left = true;
					else if (GLFW_RELEASE == action)
						camera->move.left = false;
				}
                else if (GLFW_KEY_D == key)
                {
					if (GLFW_PRESS == action)
						camera->move.right = true;
					else if (GLFW_RELEASE == action)
						camera->move.right = false;
				}

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

                if (GLFW_KEY_E == key)
                {
					if (GLFW_PRESS == action)
						camera->move.up = true;
					else if (GLFW_RELEASE == action)
						camera->move.up = false;
				}
                else if (GLFW_KEY_Q == key)
                {
					if (GLFW_PRESS == action)
						camera->move.down = true;
					else if (GLFW_RELEASE == action)
						camera->move.down = false;
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
                if (camera->mode == LOCKED_ARC_BALL || camera->mode == TOP_DOWN)
                {
                    // Lock the cursor on the middle of the screen
                    int nwidth, nheight;
                    glfwGetFramebufferSize(window, &nwidth, &nheight);
                    glfwSetCursorPos(window, float(nwidth / 2), float(nheight / 2));
                    float dx = float(xPos) - float(nwidth / 2);

                    camera->rotation.x += dx * MOUSE_SENSITIVITY;
                }
                else if (camera->mode == FlY_THROUGH)
                {
                    // Lock the cursor on the middle of the screen
                    int nwidth, nheight;
                    glfwGetFramebufferSize(window, &nwidth, &nheight);
                    glfwSetCursorPos(window, float(nwidth / 2), float(nheight / 2));
                    float xoffset = float(xPos) - float(nwidth / 2);
                    float yoffset = float(nheight / 2) - float(yPos);

                    xoffset *= MOUSE_SENSITIVITY * 10.f;
                    yoffset *= MOUSE_SENSITIVITY * 10.f;

                    camera->yaw += xoffset;
                    camera->pitch += yoffset;

                    if (camera->pitch > 89.0f)
                        camera->pitch = 89.0f;
                    if (camera->pitch < -89.0f)
                        camera->pitch = -89.0f;

                    Vec3f front;
                    front.x = cosf(radians(camera->yaw)) * cosf(radians(camera->pitch));
                    front.y = sinf(radians(camera->pitch));
                    front.z = sinf(radians(camera->yaw)) * cosf(radians(camera->pitch));
                    camera->front = normalize(front);
                }
            }
        }
    }
}