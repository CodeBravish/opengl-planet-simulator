#include "Celestial_Body.h"
#include "GravityWell.h"
#include "Renderer/Shader.h"
#include <cstddef>
#include <vector>

using glm::vec3;

std::vector<GLfloat> Body::predictPositions(const Body& me,
                                            const std::vector<Body*>& others,
                                            float timestep, unsigned int steps) {
    glm::vec3 pos = me.position;
    glm::vec3 vel = me.velocity;
    const float G = 6.67e-11f;

    std::vector<GLfloat> path;
    path.reserve(steps * 3);

    for (unsigned int i = 0; i < steps; ++i) {
        glm::vec3 netAcc(0.0f);
        for (auto* other : others) {
            if (other == &me) continue;
            glm::vec3 d = other->position - pos;
            float r2 = glm::dot(d, d);
            if (r2 <= 0.0f) continue;
            netAcc += (G * other->mass / r2) * glm::normalize(d);
        }
        vel += netAcc * timestep;
        pos += vel * timestep;
        path.push_back(pos.x);
        path.push_back(pos.y);
        path.push_back(pos.z);
    }
    return path;
}

void Body::drawOrbit(Shader& shader) {
    glBindVertexArray(this->orbitVAO);

    glm::mat4 model(1.0f);
    shader.setMat4("model", model);

    glm::vec4 color(1.0f, 0.0f, 0.0f, 0.5f);
    shader.setVec4("color", color);

    glLineWidth(1.0);
    // glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_LINE_STRIP, 0, this->orbitVertices.size() / 3);
}

void Body::updateOrbitVertexData() {
    // for (size_t i = 0; i < orbitVertices.size() - 1; i++) {
    //     this->orbitIndices.push_back(i);
    //     this->orbitIndices.push_back(i + 1);
    // }

    glBindBuffer(GL_ARRAY_BUFFER, this->orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, this->orbitVertices.size() * sizeof(GLfloat),
                 this->orbitVertices.data(), GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->orbitEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->orbitIndices.size() *
    // sizeof(GLuint),
    //              this->orbitIndices.data(), GL_STATIC_DRAW);
}

void Body::initOrbitVertexData() {
    glGenVertexArrays(1, &this->orbitVAO);
    glBindVertexArray(this->orbitVAO);
    glGenBuffers(1, &this->orbitVBO);
    // glGenBuffers(1, &this->orbitEBO);

    glBindBuffer(GL_ARRAY_BUFFER, this->orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, this->orbitVertices.size() * sizeof(GLfloat),
                 this->orbitVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->orbitEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //              this->orbitIndices.size() * sizeof(GLuint),
    //              this->orbitIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
}