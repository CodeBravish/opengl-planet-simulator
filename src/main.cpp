#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ostream>
#include <vector>

#include "Camera.h"
#include "Celestial_Body.h"
#include "GravityWell.h"
#include "Renderer/Shader.h"

#include "Settings.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void processInput(GLFWwindow *window);

const float PI = pi<float>();

// Global Settings
unsigned int screen_width = 1000;
unsigned int screen_height = 800;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5000.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

// Time
float delta_time = 0.0f;
float sim_delta_time = 0.0f;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "Planet Sim",
                                          nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;               // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(
        window, true);  // Second param install_callback=true will install GLFW
                        // callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    //

    // build and compile our shader program
    // ------------------------------------
    Shader DefaultShader("../assets/shaders/default.vert",
                         "../assets/shaders/default.frag");
    Shader PlanetShader("../assets/shaders/planet.vs",
                        "../assets/shaders/planet.fs");
    Shader GravityWellShader("../assets/shaders/gravity_well.vs",
                             "../assets/shaders/gravity_well.fs");

    vector<Planet> planets = {Planet(vec3(-2000.0f, 0.0f, 0.0f),
                                     vec3(0.0f, 0.0f, 0.03f), 5000.0e5f, 100.492f),
                              Planet(vec3(-2300.0f, 0.0f, 0.0f),
                                     vec3(0.0f, 0.0f, 0.02f), 5000.0f, 25.492f)};

    vector<Body *> bodies;
    for (auto &planet : planets) {
        bodies.push_back(&planet);
    }

    Star sun(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 250.0e8f, 696.340f);
    bodies.push_back(&sun);

    GravityWell GravityWell(50);

    // MAIN RENDER LOOP
    float prev_time = static_cast<float>(glfwGetTime());
    float accumulator = 0.0f;
    float accumulator_30fps = 0.0f;
    float fixed_time_step = 1.0f / 60.0f;
    float time_multplier = 10000.0f;

    camera.MovementSpeed = 100.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        // Timing
        float curr_time = static_cast<float>(glfwGetTime());
        delta_time = curr_time - prev_time;
        sim_delta_time = fixed_time_step * time_multplier;
        prev_time = curr_time;

        accumulator += delta_time;
        accumulator_30fps += delta_time;
        //

        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //

        // ImGUI
        ImGui::Begin("Settings");

        ImGui::Text("Simulation Controls");
        ImGui::Separator();
        ImGui::SliderFloat("Time Multplier", &time_multplier, -100000.0f, 100000.0f,
                           "%.3f sec/s");
        static bool paused = false;
        static float timestep = 0.016f;  // 60 FPS
        if (ImGui::Button(paused ? "Play" : "Pause")) {
            paused = !paused;
        }
        if (paused) sim_delta_time = 0.0f;

        ImGui::Spacing();
        ImGui::Text("Gravity Well");
        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &Settings::get().showGravityWell);
        ImGui::DragFloat("Grid Size", &GravityWell.gridSize, 10.0f, 100.0f);
        ImGui::Text("Planets");
        ImGui::Separator();
        ImGui::Checkbox("Show Orbit", &Settings::get().showOrbit);
        if (paused) sim_delta_time = 0.0f;

        ImGui::End();
        //

        // Simulation Step
        while (accumulator >= fixed_time_step) {
            for (unsigned int i = 0; i < planets.size(); i++) {
                planets[i].update(bodies, sim_delta_time);
            }

            sun.update(bodies, sim_delta_time);

            accumulator -= fixed_time_step;

            camera.LookAt(planets[1].position);

            cout << accumulator << endl;
        }
        if (accumulator_30fps >= 1.0f / 30.0f) {
            if (Settings::get().showGravityWell)
                GravityWell.updateVertexData(camera, bodies);

            accumulator_30fps = 0;
        }
        cout << "---------------------" << endl;
        //

        // Camera Matrix
        mat4 view = camera.GetViewMatrix();
        mat4 projection =
            perspective(radians(50.0f), (float)screen_width / (float)screen_height,
                        0.1f, 1000000.0f);

        // Default Shader
        DefaultShader.use();
        DefaultShader.setMat4("view", view);
        DefaultShader.setMat4("projection", projection);
        for (unsigned int i = 0; i < planets.size() && Settings::get().showOrbit;
             i++) {
            planets[i].updateOrbitVertexData();
            planets[i].drawOrbit(DefaultShader);
        }
        sun.render(DefaultShader);
        //

        // Draw Planet
        PlanetShader.use();

        PlanetShader.setMat4("view", view);
        PlanetShader.setMat4("projection", projection);

        PlanetShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        PlanetShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        PlanetShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        PlanetShader.setFloat("material.shininess", 10.0f);

        PlanetShader.setVec3("light.position", sun.position);
        PlanetShader.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
        PlanetShader.setVec3("light.diffuse", sun.color.r, sun.color.g, sun.color.b);
        PlanetShader.setVec3("light.specular", 0.1f, 0.1f, 0.1f);

        PlanetShader.setVec3("cameraPos", camera.Position);

        for (unsigned int i = 0; i < planets.size(); i++) {
            planets[i].render(PlanetShader);
        }
        //

        // Draw and Update Gravity Well
        if (Settings::get().showGravityWell) {
            GravityWellShader.use();
            GravityWellShader.setMat4("view", view);
            GravityWellShader.setMat4("projection", projection);
            GravityWell.render(GravityWellShader, camera);
        }
        //

        ImGui::Begin("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate, clearing all previously allocated GLFW resources.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
    //
}

void processInput(GLFWwindow *window) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 5000.0f;
    else
        camera.MovementSpeed = 1000.0f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, delta_time);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.isLookAtMode = !camera.isLookAtMode;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that
    // width and height will be significantly larger than specified on retina
    // displays.
    int aspect = width < height ? width : height;
    screen_width = width;
    screen_height = height;

    glViewport(0, 0, screen_width, screen_height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos;  // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (ImGui::GetIO().WantCaptureMouse) return;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}