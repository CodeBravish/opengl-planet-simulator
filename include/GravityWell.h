#pragma once

#include "Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <vector>

class GravityWell {
   public:
    unsigned int slices = 100;

    GravityWell(unsigned int slices);

    void render(const Shader& shader);

   private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint VAO;
    void initVertexData();
    void updateVertexData();
};