#include <glad.h>
#include <GLFW/glfw3.h>

#include "../third_party/imGUI/imgui.h"
#include "../third_party/imGUI/imgui_impl_glfw.h"
#include "../third_party/imGUI/imgui_impl_opengl3.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>

#include "Cubemap.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Boid.hpp"
#include "Obstacle.hpp"

#include "Terrain.hpp"
#include "Cone.hpp"
#include "loadobj.hpp"

#include "../math/mat44.hpp"
#include "../math/other.hpp"


extern "C"  {
    // Force the use of the NVIDIA GPU on laptops with switchable graphics
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
}

namespace {
    // Global constant variables
    constexpr unsigned int WINDOW_WIDTH = 1280;
    constexpr unsigned int WINDOW_HEIGHT = 720;

    constexpr float CAMERA_MOVEMENT = 50.f;
    constexpr float MOUSE_SENSITIVITY = 0.005f;

    constexpr unsigned int LOCKED_ARC_BALL = 1;
    constexpr unsigned int TOP_DOWN = 2;
    constexpr unsigned int FlY_THROUGH = 3;
    constexpr unsigned int THIRD_PERSON = 4;

    constexpr Vec3f SIMULATION_SIZE = { 100.f, 50.f, 100.f };

    constexpr unsigned int NO_DIRECTION = 0;
    constexpr unsigned int DIRECTION_GIVEN = 1;
    constexpr unsigned int POINT_GIVEN = 2;

    // Global variables changeable in the GUI
    unsigned int boidControl = NO_DIRECTION;

    float boidSpeed = 40.f;
    float boidVisionRange = 12.f;
    float boidVisionAngle = 150.f;

    int boidsCount = 1000;

    float cohesionStrength = 1.f;
    float alignmentStrength = 1.f;
    float separationStrength = 3.f;

    unsigned int boidToFollow = 0;

    // Options for the simulation
    bool paused = true;
    bool showGUI = true;
    bool technicalView = false;

    struct CameraState {
        bool active;
        struct move { bool forward, backwards, left, right, up, down; float speed; } move;

        Vec2f rotation = { 0.f, 0.f };
        Vec3f position = { 0.f, 0.f, 150.f };
        unsigned int mode = LOCKED_ARC_BALL; // default mode

        float yaw = -90.f;
        float pitch = 0.f;

        Vec3f front = { 0.f, 0.f, -1.f };
        Vec3f up = { 0.f, 1.f, 0.f };
    };

    Light light = {
        { 0.f, 100.f, 0.f }, // position
        { 0.1f, 0.1f, 0.1f }, // ambient
        { 0.50980f, 0.88235f, 0.89020f }, // color (sunlight sRGB 0-1.0)
        10000.f // strength
    };

    // Callaback definitions for glfw
    void error_callback(int, const char*);
    void key_callback(GLFWwindow*, int, int, int, int);
    void cursor_position_callback(GLFWwindow*, double, double);
    void mouse_button_callback(GLFWwindow*, int, int, int);

    // Vectors for controlling the boids
    Vec3f userInputLocation = { 0.f, 0.f, 0.f };
    Vec3f userInputDirection = { 0.f, 0.f, 0.f };

    // Mouse input
    bool rightClick = false;
    bool leftClick = false;

    // Fish animation
    float tailAngle = 0.f;
    float tailSpeed = 0.01f;
}


int main() {
    // Initialize glfw
    if (!glfwInit()) {
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

    if (window == NULL) {
        glfwTerminate();
        throw std::exception("Failed to create GLFW window");
    }

    // Initialize camera
    CameraState camera{};

    // Set up glfw event handling
    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, &key_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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
    
    // Set up shader for the skybox
    Shader CubemapShader("assets/shaders/Cubemap.vert", "assets/shaders/Cubemap.frag");
    const char* faces[6] = { "assets/textures/right.jpg",
                            "assets/textures/left.jpg",
                            "assets/textures/top.jpg",
                            "assets/textures/bottom.jpg",
                            "assets/textures/front.jpg",
                            "assets/textures/back.jpg" };
    Cubemap cubemap(faces);

    // Set up shaders for model rendering
    Shader SimpleShader("assets/shaders/BlinnPhongSimple.vert", "assets/shaders/BlinnPhongSimple.frag");
    Shader MultiMaterialShader("assets/shaders/BlinnPhongMultiMat.vert", "assets/shaders/BlinnPhongMultiMat.frag");
    GLuint shadersInUse[] = { SimpleShader.data.shaderProgram, MultiMaterialShader.data.shaderProgram };

    // ----------------------------- Define objects ----------------------------- //

    // Terrain with material
    Material terrainMat = Material{ rgb_to_linear(Vec3f{ 172, 150, 83 }), rgb_to_linear(Vec3f{ 189, 171, 117 }), rgb_to_linear(Vec3f{ 205, 192, 152 })};
    Model terrain = generate_terrain("assets/textures/heightmap.png", terrainMat, make_scaling({ 0.0078f, 0.0005f, 0.0078f })); // Scaled to a 1 unit size
    float maxHeight = 0.f;
    for (Vertex& v : terrain.vertices) {
        if (v.positions.y > maxHeight) maxHeight = v.positions.y;
    }
    maxHeight = maxHeight * SIMULATION_SIZE.y;


    // Obstacles meshes loaded from obj files
    Model box = Model(load_wavefront_obj("assets/models/box.obj"));
    Model sphere = Model(load_wavefront_obj("assets/models/sphere.obj"));
    Model columns = Model(load_wavefront_obj("assets/models/AllColumns.obj"));
    Model rocks = Model(load_wavefront_obj("assets/models/AllRocks.obj"));

    // Abstract obstacle vector
    std::vector<Obstacle*> obstacles;

    // Columns on left side
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 18.f, 5.f }, Vec3f{ 4.f, 24.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 18.f, -25.f }, Vec3f{ 4.f, 24.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 18.f, -55.f }, Vec3f{ 4.f, 24.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 42.6f, -24.f }, Vec3f{ 4.f, 2.f, 38.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 0.f, 35.f }, Vec3f{ 4.f, 5.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 4.f, 57.f }, Vec3f{ 4.f, 8.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 0.1f, 49.9f }, Vec3f{ 4.f, 4.6f, 3.1f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -9.f, -3.f, 44.f }, Vec3f{ 3.5f, 3.f, 12.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -15.2f, 18.f, 65.f }, Vec3f{ 4.f, 24.f, 4.f }));

    // Columns on right side
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 67.5f, 18.f, -55.f }, Vec3f{ 4.f, 24.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 46.f, -2.5f, -26.f }, Vec3f{ 26.f, 4.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 67.5f, 7.6f, 5.f }, Vec3f{ 4.f, 13.5f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 67.5f, 18.f, 35.f }, Vec3f{ 4.f, 24.f, 4.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 67.5f, 42.6f, 48.f }, Vec3f{ 4.f, 2.f, 23.f }));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ 67.5f, 17.f, 65.f }, Vec3f{ 4.f, 24.f, 4.f }));

    // statue in the middle
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ 26.5f, 1.f, -56.f }, 9.f));
    
    // rocks
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ -60.f, -8.f, -58.f }, 35.f));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -62.8f, 2.4f, -20.f }, Vec3f{ 7.f, 9.f, 10.f }));
    obstacles.push_back(new SphereObstacle(&sphere, Vec3f{ -68.f, -8.7f, 40.f }, 29.f));
    obstacles.push_back(new BoxObstacle(&box, Vec3f{ -60.9f, -2.3f, 10.f }, Vec3f{ 9.3f, 3.7f, 7.f }));


    // Fish mesh loaded from obj files
    Model fish = load_wavefront_obj("assets/models/fish.obj");
    
    // Cone mesh to represent the boids in technical view
    Model cone = generate_cone(16, {}, make_scaling({ 3.f, 1.f, 1.f }));

    // Initialize a number of boidsCount boids
    std::vector<Boid*> boids;
    srand((unsigned int)(time(NULL)));
    for (int i = 0; i < boidsCount; i++)
    {
	    boids.push_back(new Boid(obstacles));
    }

    //ImGUI setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Rules initialization for the boids
    Vec3f cohesion = { 0.f, 0.f, 0.f };
    Vec3f alignment = { 0.f, 0.f, 0.f };
    Vec3f separation = { 0.f, 0.f, 0.f };
    Vec3f avoid = { 0.f, 0.f, 0.f };

    // Start the rendering loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update number of boids if changed by the GUI
        while (boidsCount > boids.size()) {
            boids.push_back(new Boid(obstacles));
        }

        // If the number of boids is decreased, delete the last boids
        while (boidsCount < boids.size()) {
            boids.pop_back();
        }

        // If the user is using the third person camera and the number of boids is decreased, 
        // deleting the currently viewed boid defaults to the first boid or to the locked arc 
        // ball camera if there is no boid left
        if (boids.size() == 0 && camera.mode == THIRD_PERSON) {
            camera.mode = LOCKED_ARC_BALL;
            camera.position.z = 150.f;
            boidToFollow = 0;
        }
        else if(boids.size() > 0 && boidToFollow > boids.size()) boidToFollow = 0;

        // Set viewport to current window size
        int nwidth, nheight;
        glfwGetFramebufferSize(window, &nwidth, &nheight);

        // if the window is minimized, wait for it to be restored
        while (0 == nwidth || 0 == nheight) {
            glfwWaitEvents();
            glfwGetFramebufferSize(window, &nwidth, &nheight);
        }

        glViewport(0, 0, nwidth, nheight);

        // Update time dt between frames
        auto const now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - last).count();
        last = now;

        // Camera movement
        float measurmentUnit = CAMERA_MOVEMENT * dt * camera.move.speed;
        // Update camera state based on user input
        if (camera.mode == LOCKED_ARC_BALL || camera.mode == TOP_DOWN || camera.mode == THIRD_PERSON) {
            if (camera.move.forward)
                camera.position.z -= measurmentUnit;
            else if (camera.move.backwards)
                camera.position.z += measurmentUnit;

            if (camera.position.z <= 0.1f)
                camera.position.z = 0.1f;
        }
        else if (camera.mode == FlY_THROUGH) {
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
        if (camera.mode == LOCKED_ARC_BALL) {
            Mat44f Rx = make_rotation_x(radians(30.f));
            // Defaults to the initial rotation if the user is trying to guide the boids
            if (boidControl == POINT_GIVEN && !camera.active)
                camera.rotation.x = 0.f;
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry;
        }
        else if (camera.mode == FlY_THROUGH) {
            world2camera = look_at(camera.position, camera.front, camera.up);
        }
        else if (camera.mode == TOP_DOWN)
        {
            Mat44f Rx = make_rotation_x(radians(90.f));
            // Defaults to the initial rotation if the user is trying to guide the boids
            if (boidControl == POINT_GIVEN && !camera.active)
                camera.rotation.x = 0.f;
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry;
        }
        else if (camera.mode == THIRD_PERSON)
        {
            // Translate camera to boid's current position
            Mat44f T1 = make_translation(-boids.at(boidToFollow)->currentPosition);

            // Rotate camera around the object and translate from/to it to zoom
            Mat44f Rx = make_rotation_x(camera.rotation.y);
            Mat44f Ry = make_rotation_y(camera.rotation.x);
            Mat44f T = make_translation({ 0.f, 0.f, -camera.position.z });
            world2camera = T * Rx * Ry * T1;
        }


        // Set up projection to screen
        Mat44f projection = make_perspective_projection(
            60.f * 3.1415926f / 180.f, // fov = 60 degrees
            nwidth / float(nheight),
            0.1f, 100.0f
        );

        Mat44f world2projection = projection * world2camera;

        // ImGui setup for the GUI
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Disable mouse cursor when moving the camera
            if(camera.active)
	            ImGui::SetMouseCursor(ImGuiMouseCursor_None);

            // Render GUI
            ImGui::Begin("Simulation Parameters");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("Pause - PRESS [SPACE]", &paused);
            ImGui::SameLine();
            ImGui::Checkbox("Technical View [T]", &technicalView);
            ImGui::Checkbox("Switch GUI on/off [G]", &showGUI);
            if (ImGui::CollapsingHeader("Boid settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("Boid Count", &boidsCount, 0, 3000);
                ImGui::SliderFloat("Boid Speed", &boidSpeed, 0.f, 100.f);
                ImGui::SliderFloat("Boid Vision Range", &boidVisionRange, 0.f, 15.f);
                ImGui::SliderFloat("Boid Vision Angle", &boidVisionAngle, 0.f, 180.f);
                if (ImGui::Button("Default parameters")) {
                    boidSpeed = 40.f;
                    boidVisionRange = 12.f;
                    boidVisionAngle = 150.f;
                    boidsCount = 1000;
                }
                ImGui::Separator();
                ImGui::Text("Rules:");
                ImGui::SliderFloat("Cohesion", &cohesionStrength, 0.f, 5.f);
                ImGui::SliderFloat("Alignment", &alignmentStrength, 0.f, 5.f);
                ImGui::SliderFloat("Separation", &separationStrength, 0.f, 5.f);
                if (ImGui::Button("Default rule strengths")) {
                    cohesionStrength = 1.f;
                    alignmentStrength = 1.f;
                    separationStrength = 3.f;
                }
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Camera controls (Instructions)", false)) {
                ImGui::Columns(2, "col0");
                ImGui::SetColumnWidth(0, 250);
                ImGui::Text("Increase camera speed");
                ImGui::Text("Decrease camera speed");

                ImGui::NextColumn();
                ImGui::Text("PRESS [SHIFT]");
                ImGui::Text("PRESS [CTRL]");
                ImGui::Columns(1);
                ImGui::Separator();


                ImGui::Columns(2, "col1");
                ImGui::SetColumnWidth(0, 250);
                ImGui::Text("Camera 1: Locked height ArcBall");
                ImGui::Text("Zoom in/out");
                ImGui::Text("Rotate camera left/right");

                ImGui::NextColumn();
                ImGui::Text("PRESS [1]");
                ImGui::Text("PRESS [W]/[S]");
                ImGui::Text("MOVE MOUSE");
                ImGui::Columns(1);
                ImGui::Separator();

                ImGui::Columns(2, "col2");
                ImGui::SetColumnWidth(0, 250);
                ImGui::Text("Camera 2: Top-Down View");
                ImGui::Text("Zoom in/out");
                ImGui::Text("Rotate camera left/right");

                ImGui::NextColumn();
                ImGui::Text("PRESS [2]");
                ImGui::Text("PRESS [W]/[S]");
                ImGui::Text("MOVE MOUSE");
                ImGui::Columns(1);
                ImGui::Separator();

                ImGui::Columns(2, "col3");
                ImGui::SetColumnWidth(0, 250);
                ImGui::Text("Camera 3: Fly-Through");
                ImGui::Text("Move forward/backwards");
                ImGui::Text("Move left/right");
                ImGui::Text("Move down/up");
                ImGui::Text("Rotate camera left/right/up/down");

                ImGui::NextColumn();
                ImGui::Text("PRESS [3]");
                ImGui::Text("PRESS [W]/[S]");
                ImGui::Text("PRESS [A]/[D]");
                ImGui::Text("PRESS [Q]/[E]");
                ImGui::Text("MOVE MOUSE");
                ImGui::Columns(1);
                ImGui::Separator();

                ImGui::Columns(2, "col4");
                ImGui::SetColumnWidth(0, 250);
                ImGui::Text("Camera 4: Third-Person");
                ImGui::Text("Zoom in/out");
                ImGui::Text("Rotate camera left/right/up/down");

                ImGui::NextColumn();
                ImGui::Text("PRESS [4]");
                ImGui::Text("PRESS [W]/[S]");
                ImGui::Text("MOVE MOUSE");
                ImGui::Columns(1);
                ImGui::Separator();
            }
            if(ImGui::Button("No direction")) { boidControl = NO_DIRECTION; userInputDirection = { 0.f, 0.f, 0.f }; }
            ImGui::SameLine();
            if (ImGui::Button("Go in direction")) { boidControl = DIRECTION_GIVEN; userInputDirection = { 0.f, 0.f, 0.f }; }
            ImGui::SameLine();
            if (ImGui::Button("Go to point")) {
                boidControl = POINT_GIVEN;
                if (camera.mode == THIRD_PERSON || camera.mode == FlY_THROUGH) {
                    camera.mode = LOCKED_ARC_BALL;
                    camera.position.z = 150.f;
                }
            }
            if (boidControl == POINT_GIVEN) {
                ImGui::Text("Camera Locked while controlling the target point!");
                ImGui::Text("To move the target point, use these sliders or:");
                ImGui::Text("Hold [LEFT CLICK] to move it on the Y axis.");
                ImGui::Text("Hold [RIGHT CLICK] to move on the XZ plane.");
                ImGui::SliderFloat("Point X", &userInputLocation.x, -SIMULATION_SIZE.x, SIMULATION_SIZE.x);
                ImGui::SliderFloat("Point Y", &userInputLocation.y, 0.f, SIMULATION_SIZE.y);
                ImGui::SliderFloat("Point Z", &userInputLocation.z, -SIMULATION_SIZE.z, SIMULATION_SIZE.z);
                if (ImGui::Button("Default target (Origin)")) {
                    userInputLocation = { 0.f, 0.f, 0.f };
                }
            }
            else if (boidControl == DIRECTION_GIVEN) {
                ImGui::SliderFloat("Direction X", &userInputDirection.x, -1.f, 1.f );
                ImGui::SliderFloat("Direction Y", &userInputDirection.y, -1.f, 1.f );
                ImGui::SliderFloat("Direction Z", &userInputDirection.z, -1.f, 1.f );
                if (ImGui::Button("Default (No direction)")) {
					userInputDirection = { 0.f, 0.f, 0.f };
				}
            }

            ImGui::End();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set terrain transforms
        terrain.model2world = make_translation({ 0.f, -maxHeight, 0.f }) * make_scaling(SIMULATION_SIZE);

        // Simulation parameters
        float movementSpeed = dt * boidSpeed;
        float turnSharpness = movementSpeed * 0.2f;

        // If the simulation is running, animate the boids
        if (!technicalView && !paused) {
            tailAngle += tailSpeed;
            if (tailAngle >= 0.2f || tailAngle <= -0.2f) tailSpeed = -tailSpeed;
        }

        // Apply boids algorithm
        for (auto boid : boids) {
            if (!paused) {
                std::vector<Boid*> neighbours = boid->findNeighbours(boids, boidVisionRange, boidVisionAngle);
                cohesion = boid->applyCohesion(neighbours, cohesionStrength);
                alignment = boid->applyAlignment(neighbours, alignmentStrength);
                separation = boid->applySeparation(neighbours, separationStrength, boidVisionRange);
                avoid = boid->avoidEdges(2.f) + boid->avoidObstacles(obstacles, 3.f);

                if (boidControl == POINT_GIVEN) {
                    userInputDirection = normalize(userInputLocation - boid->currentPosition);
                }

                boid->setTargetDirection(normalize(boid->currentDirection +
                    cohesion + alignment + separation + userInputDirection) + avoid);
                boid->updateDirection(movementSpeed, turnSharpness);
            }
            // Render boids with the animated fish model or the cone (technical view)
            if (!technicalView) {
                Mat44f animation = make_shear_x(0.f, tailAngle);
                fish.render(camera.position, light, world2projection, boid->model2world*animation, shadersInUse);
            }
            else
                cone.render(camera.position, light, world2projection, boid->model2world, shadersInUse);
        }

        // Render terrain (as wireframe if in technical view mode)
        if(technicalView)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        terrain.render(camera.position, light, world2projection, terrain.model2world, shadersInUse);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Render obstacles: columns and rocks
        columns.render(camera.position, light, world2projection, columns.model2world, shadersInUse);
        rocks.render(camera.position, light, world2projection, rocks.model2world, shadersInUse);

        // Render red sphere at the target point given by the user
        if(boidControl == POINT_GIVEN)
            sphere.render(camera.position, light, world2projection, make_translation(userInputLocation), shadersInUse);

        // Render obstacle hitboxes (bounding volumes) if in technical view mode
        if (technicalView) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            for (auto obstacle : obstacles) {
                if(obstacle->model)
                    obstacle->model->render(camera.position, light, world2projection, obstacle->model2world, shadersInUse);
		    }
            glDisable(GL_BLEND);
        }
        else // Render cubemap if not in technical view mode
            cubemap.render(projection, world2camera, CubemapShader.data.shaderProgram);

        glBindVertexArray(0);
        glUseProgram(0);

        ImGui::Render();
        
        // Render GUI if enabled
        if (showGUI) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }

    // End ImGui processes
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Clean-up
    for (auto boid : boids) {
		delete boid;
	}

    for (auto obstacle : obstacles) {
		delete obstacle;
	}

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


// Setup for GLFW callbacks
namespace {
    void error_callback(int code, const char* description) {
        std::fprintf(stderr, "GLFW error: %s (%d)\n", description, code);
    }

    // Callback function for keyboard input
    void key_callback(GLFWwindow* window, int key, int, int action, int mods) {
        if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        if (GLFW_KEY_SPACE == key && GLFW_PRESS == action) {
            paused = !paused;
        }

        if (GLFW_KEY_T == key && GLFW_PRESS == action) {
            technicalView = !technicalView;
        }

        if (GLFW_KEY_G == key && GLFW_PRESS == action) {
            showGUI = !showGUI;
        }

        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window))) {
            // Space toggles camera
            if (GLFW_KEY_C == key && GLFW_PRESS == action) {
                camera->active = !camera->active;

                if (camera->active)
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                else
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // Camera mode
            if (GLFW_KEY_1 == key && GLFW_PRESS == action) {
				camera->mode = LOCKED_ARC_BALL;
                camera->position.z = 150.f;
            }
            else if (GLFW_KEY_2 == key && GLFW_PRESS == action) {
				camera->mode = TOP_DOWN;
                camera->position.z = 150.f;
            }
            else if (GLFW_KEY_3 == key && GLFW_PRESS == action) {
				camera->mode = FlY_THROUGH;
                camera->position = { 0.f, 25.f, 150.f};
                camera->yaw = -90.f;
                camera->pitch = 0.f;
                camera->front = { 0.f, 0.f, -1.f };
            }
            else if (GLFW_KEY_4 == key && GLFW_PRESS == action) {
                if (boidsCount != 0) {
                    boidToFollow = rand() % boidsCount;
				    camera->mode = THIRD_PERSON;
                    camera->position.z = 10.f;
                }
            }

            // Camera controls if camera is active
            if (camera->active) {
                // Movement controls
                if (GLFW_KEY_A == key) {
					if (GLFW_PRESS == action)
						camera->move.left = true;
					else if (GLFW_RELEASE == action)
						camera->move.left = false;
				}
                else if (GLFW_KEY_D == key) {
					if (GLFW_PRESS == action)
						camera->move.right = true;
					else if (GLFW_RELEASE == action)
						camera->move.right = false;
				}

                if (GLFW_KEY_W == key) {
                    if (GLFW_PRESS == action)
                        camera->move.forward = true;
                    else if (GLFW_RELEASE == action)
                        camera->move.forward = false;
                }
                else if (GLFW_KEY_S == key) {
                    if (GLFW_PRESS == action)
                        camera->move.backwards = true;
                    else if (GLFW_RELEASE == action)
                        camera->move.backwards = false;
                }

                if (GLFW_KEY_E == key) {
					if (GLFW_PRESS == action)
						camera->move.up = true;
					else if (GLFW_RELEASE == action)
						camera->move.up = false;
				}
                else if (GLFW_KEY_Q == key) {
					if (GLFW_PRESS == action)
						camera->move.down = true;
					else if (GLFW_RELEASE == action)
						camera->move.down = false;
				}

                if (GLFW_MOD_SHIFT == mods)
                    camera->move.speed = 2.5f;
                else if (GLFW_MOD_CONTROL == mods)
                    camera->move.speed = 0.2f;
                else
                    camera->move.speed = 1.f;
            }
        }
    }

    // Callback function for mouse buttons input
    void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window))) {
            if (!camera->active && boidControl == POINT_GIVEN) {
                if (GLFW_MOUSE_BUTTON_RIGHT == button) {
                    if (GLFW_PRESS == action)
                        rightClick = true;
                    else if (GLFW_RELEASE == action)
                        rightClick = false;
                }
                else if (GLFW_MOUSE_BUTTON_LEFT == button) {
                    if (GLFW_PRESS == action)
                        leftClick = true;
                    else if (GLFW_RELEASE == action)
                        leftClick = false;
                }
			}
		}
	}

    // Callback function for mouse movement
    void cursor_position_callback(GLFWwindow* window, double xPos, double yPos)
    {
        if (auto* camera = static_cast<CameraState*>(glfwGetWindowUserPointer(window))) {
            if (camera->active) {
                if (camera->mode == LOCKED_ARC_BALL || camera->mode == TOP_DOWN) {
                    // Lock the cursor on the middle of the screen
                    int nwidth, nheight;
                    glfwGetFramebufferSize(window, &nwidth, &nheight);
                    glfwSetCursorPos(window, float(nwidth / 2), float(nheight / 2));
                    float dx = float(xPos) - float(nwidth / 2);

                    camera->rotation.x += dx * MOUSE_SENSITIVITY;
                }
                else if (camera->mode == THIRD_PERSON) {
                    // Lock the cursor on the middle of the screen
                    int nwidth, nheight;
                    glfwGetFramebufferSize(window, &nwidth, &nheight);
                    glfwSetCursorPos(window, float(nwidth / 2), float(nheight / 2));
                    float dx = float(xPos) - float(nwidth / 2);
                    float dy = float(yPos) - float(nheight / 2);

                    camera->rotation.x += dx * MOUSE_SENSITIVITY;
                    camera->rotation.y += dy * MOUSE_SENSITIVITY;
                    if (camera->rotation.y > radians(90))
                        camera->rotation.y = radians(90);
                    else if (camera->rotation.y < radians(-90))
                        camera->rotation.y = radians(-90);
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
            else {
                if(camera->mode == LOCKED_ARC_BALL || camera->mode == TOP_DOWN)
                    // If right click is pressed, change userInputLocation on XZ plane
                    if (rightClick) {
                        int nwidth, nheight;
					    glfwGetFramebufferSize(window, &nwidth, &nheight);
					    float x = float(xPos) / float(nwidth);
					    float y = float(yPos) / float(nheight);
					    userInputLocation.x = x * 2 * SIMULATION_SIZE.x - SIMULATION_SIZE.x;
					    userInputLocation.z = y * 2 * SIMULATION_SIZE.z - SIMULATION_SIZE.z;

                        if (userInputLocation.x > SIMULATION_SIZE.x) userInputLocation.x = SIMULATION_SIZE.x;
                        else if (userInputLocation.x < -SIMULATION_SIZE.x) userInputLocation.x = -SIMULATION_SIZE.x;
                
                        if (userInputLocation.z > SIMULATION_SIZE.z) userInputLocation.z = SIMULATION_SIZE.z;
                        else if (userInputLocation.z < -SIMULATION_SIZE.z) userInputLocation.z = -SIMULATION_SIZE.z;
                    }
                    // If left click is pressed, change userInputLocation on Y axis
                    else if (leftClick) {
                        int nwidth, nheight;
                        glfwGetFramebufferSize(window, &nwidth, &nheight);
                        float y = float(yPos) / float(nheight);
                        userInputLocation.y = y * -50.f + 50.f;
                        if (userInputLocation.y > 50.f) userInputLocation.y = 50.f;
                        else if (userInputLocation.y < 0.f) userInputLocation.y = 0.f;
                    }
            }
        }
    }
}