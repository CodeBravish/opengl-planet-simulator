#include "GravityWell.h"
#include <glm/fwd.hpp>

GravityWell::GravityWell(GLfloat gridSize) : gridSize(gridSize) { initVertexData(); }

void GravityWell::render(const Shader& shader) {
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(this->VAO);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    shader.setMat4("model", model);

    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    shader.setVec4("color", color);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

void GravityWell::updateVertexData() {
    this->vertices.clear();
    this->indices.clear();

    GLfloat x, y = 0.0f, z;
    unsigned int slices = static_cast<unsigned int>(mapSize / gridSize);

    for (unsigned int i = 0; i < slices; i++) {
        x = i * gridSize;

        for (unsigned int j = 0; j < slices; j++) {
            z = j * gridSize;
            this->vertices.push_back(x);
            this->vertices.push_back(y);
            this->vertices.push_back(z);
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