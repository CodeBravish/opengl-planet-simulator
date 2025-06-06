#include "GravityWell.h"
#include "Renderer/Shader.h"
#include <cmath>
#include <glm/fwd.hpp>

GravityWell::GravityWell(GLfloat gridSize) : gridSize(gridSize) { initVertexData(); }

void GravityWell::render(const Shader& shader, const Camera& camera) {
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(this->VAO);

    // glm::vec3 cam = camera.Position;
    // glm::vec3 gridSnap(cam.x - fmodf(cam.x, gridSize), 0.0,
    //                    cam.z - fmodf(cam.z, gridSize));

    glm::mat4 model(1.0f);
    // model = glm::translate(model, glm::vec3(-mapSize / 2, 0.0, -mapSize / 2));
    // model = glm::translate(model, gridSnap);

    shader.setMat4("model", model);

    glm::vec3 gridColor(1.0f, 1.0f, 1.0f);

    shader.setVec3("gridColor", gridColor);
    shader.setFloat("mapSize", this->mapSize);
    shader.setVec3("cameraPos", camera.Position);

    glLineWidth(1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

void GravityWell::updateVertexData(const Camera& camera,
                                   const std::vector<Body*>& other_bodies) {
    this->vertices.clear();
    this->indices.clear();

    glm::vec3 cam = camera.Position;
    glm::vec3 gridSnap(cam.x - fmodf(cam.x, gridSize), 0.0f,
                       cam.z - fmodf(cam.z, gridSize));

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(-mapSize / 2.0f, 0.0f, -mapSize / 2.0f));
    model = glm::translate(model, gridSnap);

    GLfloat x, y = 50.0f, z;
    unsigned int slices = static_cast<unsigned int>(mapSize / gridSize);

    for (unsigned int i = 0; i < slices; i++) {
        x = i * gridSize;

        for (unsigned int j = 0; j < slices; j++) {
            z = j * gridSize;
            glm::vec3 worldVertex = model * glm::vec4(x, y, z, 1.0f);

            const float G = 6.67e-11f;

            for (unsigned int i = 0; i < other_bodies.size(); i++) {
                const Body* other_body = other_bodies[i];

                glm::vec3 distance_vector = other_body->position - worldVertex;
                float distance_mag = glm::length(distance_vector);

                if (distance_mag == 0) continue;

                // Calculate Gravitional Acceleration from Each Planet
                float acceleration = G * other_body->mass /
                                    (distance_mag);

                worldVertex.y -= acceleration * 2.0e5f;
            }

            this->vertices.push_back(worldVertex.x);
            this->vertices.push_back(worldVertex.y);
            this->vertices.push_back(worldVertex.z);
        }
    }

    unsigned int index = 0;
    for (unsigned int row = 0; row < slices; row++) {
        for (unsigned int col = 0; col < slices; col++) {
            if (col + 1 < slices) {
                indices.push_back(index);
                indices.push_back(index + 1);
            }

            if (row + 1 < slices) {
                indices.push_back(index);
                indices.push_back(index + slices);
            }
            index++;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertices.size() * sizeof(GLfloat),
    //                 this->vertices.data());
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat),
                 this->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
                 this->indices.data(), GL_STATIC_DRAW);
}

void GravityWell::initVertexData() {
    vertices.clear();
    indices.clear();

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat),
                 this->vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
                 this->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
}