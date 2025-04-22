#pragma once

#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <vector>

#include "Camera.h"
#include "Renderer/Shader.h"

class GravityWell {
   public:
    GLfloat gridSize;
    GLfloat mapSize = 10000.0f;
    GravityWell(GLfloat gridSize);

    void render(const Shader& shader);
    void updateVertexData();

   private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint VAO, VBO, EBO;
    void initVertexData();
};