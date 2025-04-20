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

#include "Celestial_Body.h"
#include "Renderer/Shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const float PI = pi<float>();

// settings
unsigned int screen_width = 1070;
unsigned int screen_height = 1070;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10000.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cam_yaw = 0.0;
float cam_pitch = 0.0;

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
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    // ##########

    // build and compile our shader program
    // ------------------------------------
    Shader DefaultShader("../assets/shaders/default.vert",
                         "../assets/shaders/default.frag");

    DefaultShader.use();

    vector<Planet> planets = {
        Planet(vec3(-1500.0f, 0.0f, 0.0f), vec3(0.0f, 0.03f, 0.0f), 50.0f, 71.492f)};

    // planets.emplace_back(vec3(2.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
    // planets.emplace_back(vec3(-2.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

    // Planet planet1(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));

    vector<Body *> bodies;

    for (auto &planet : planets) {
        bodies.push_back(&planet);
    }

    Star sun(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 10.0e8f, 696.340f);

    bodies.push_back(&sun);

    // render loop
    // -----------
    DefaultShader.use();

    float prev_time = static_cast<float>(glfwGetTime());
    float time_multplier = 1.0f;

    while (!glfwWindowShouldClose(window)) {
        float curr_time = static_cast<float>(glfwGetTime());
        float delta_time = (curr_time - prev_time) * time_multplier;
        prev_time = curr_time;

        processInput(window);

        // Clear Screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ###########

        ImGui::Begin("Menu");

        ImGui::DragFloat("s/s", &time_multplier, 1.0f);

        ImGui::DragFloat3("Camera Postion", glm::value_ptr(cameraPos), 0.01f);

        ImGui::End();

        // Camera
        mat4 projection = mat4(1.0f);

        glm::mat4 view;

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        projection =
            perspective(radians(50.0f), (float)screen_width / (float)screen_height,
                        0.1f, 1000000.0f);

        unsigned int viewLoc = glGetUniformLocation(DefaultShader.ID, "view");
        unsigned int projectionLoc =
            glGetUniformLocation(DefaultShader.ID, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
        //

        // Draw Planet
        for (unsigned int i = 0; i < planets.size(); i++) {
            planets[i].update(bodies, delta_time);
            planets[i].render(DefaultShader);
        }
        sun.update(bodies, delta_time);
        sun.render(DefaultShader);

        cout << endl << endl;

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // #################
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const vec3 horizontalFront = normalize(cameraFront * vec3(1.0f, 0.0f, 1.0f));
    const vec3 cameraUpNorm = normalize(cameraUp);

    const float cameraSpeed = 100.0;  // adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -=
            glm::normalize(glm::cross(horizontalFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos +=
            glm::normalize(glm::cross(horizontalFront, cameraUp)) * cameraSpeed;

    // cout << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << "," <<
    // endl;
}

// glfw: whenever the window size changed (by OS or user resize) this
// callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that
    // width and height will be significantly larger than specified on retina
    // displays.
    int aspect = width < height ? width : height;
    screen_width = width;
    screen_height = height;

    glViewport(0, 0, screen_width, screen_height);
}

float lastX = 0.0, lastY = 0.0;
bool first_mouse = true;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos;  // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam_yaw += xoffset;
    cam_pitch += yoffset;

    if (cam_pitch > 89.0f) cam_pitch = 89.0f;
    if (cam_pitch < -89.0f) cam_pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(radians(cam_yaw)) * cos(radians(cam_pitch));
    direction.y = sin(radians(cam_pitch));
    direction.z = sin(radians(cam_yaw)) * cos(radians(cam_pitch));
    cameraFront = glm::normalize(direction);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    const float cameraSpeed = 1000.0;  // adjust accordingly

    cameraPos += cameraFront * static_cast<float>(yoffset) * cameraSpeed;
}