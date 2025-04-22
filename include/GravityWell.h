#pragma once

#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <vector>

#include "Camera.h"
#include "Celestial_Body.h"
#include "Renderer/Shader.h"

class GravityWell {
   public:
    GLfloat gridSize;
    GLfloat mapSize = 10000.0f;
    GravityWell(GLfloat gridSize);

    void render(const Shader& shader, const Camera& camera);
    void updateVertexData(const Camera& camera, const std::vector<Body*>& other_bodies);

   private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint VAO, VBO, EBO;
    void initVertexData();
};