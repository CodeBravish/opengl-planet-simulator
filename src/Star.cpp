#include "Celestial_Body.h"
#include "GravityWell.h"
#include "Renderer/Shader.h"
#include "utils/Formating.h"

#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <ostream>
#include <vector>

using glm::mat4;
using glm::vec3;
using glm::vec4;
using namespace std;

Star::Star(vec3 position, vec3 velocity, float mass, float radius)
    : Body(position, velocity, mass), radius(radius) {
    initVertexData();
}

void Star::render(const Shader& shader) {
    glBindVertexArray(this->VAO);

    mat4 model = mat4(1.0f);

    model = glm::translate(model, this->position);
    model = glm::scale(model, vec3(this->radius));
    model = glm::rotate(model, (float)glfwGetTime() * this->angular_speed,
                        vec3(0.0f, 0.0f, 1.0f));

    shader.setMat4("model", model);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Color and Grid on Planet
    shader.setVec3("color", this->color);

    // 1. Draw filled sphere
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);
}

void Star::initVertexData() {
    std::vector<GLfloat> vertices;

    float radius = 1;
    unsigned int sectorCount = 128;
    unsigned int stackCount = 64;

    float x, y, z, xy;  // vertex position

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);       // r * cos(u)
        z = radius * sinf(stackAngle);        // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;  // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // std::vector<int> lineIndices;
    int k1, k2;

    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);  // beginning of current stack
        k2 = k1 + sectorCount + 1;   // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                this->indices.push_back(k1);
                this->indices.push_back(k2);
                this->indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                this->indices.push_back(k1 + 1);
                this->indices.push_back(k2);
                this->indices.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            this->line_indices.push_back(k1);
            this->line_indices.push_back(k2);
            if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                this->line_indices.push_back(k1);
                this->line_indices.push_back(k1 + 1);
            }
        }
    }

    GLuint VBO, EBO;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
}

void Star::update(const vector<Body*>& other_bodies, float delta_time) {
    const float G = 6.67e-11f;

    for (unsigned int i = 0; i < other_bodies.size(); i++) {
        const Body* other_body = other_bodies[i];

        if (this == other_body) continue;

        vec3 distance_vector = other_body->position - this->position;
        vec3 distance_normal = glm::normalize(distance_vector);
        float distance_mag = glm::length(distance_vector);

        if (distance_mag == 0) continue;

        // Calculate Gravitional Acceleration from Each Planet
        vec3 acceleration =
            distance_normal * G * other_body->mass / (distance_mag * distance_mag);

        this->velocity += acceleration * delta_time;
    }

    this->position += this->velocity * delta_time;
}