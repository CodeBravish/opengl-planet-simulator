#ifndef PLANET_H
#define PLANET_H

#include "Renderer/Shader.h"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/trigonometric.hpp>
#include <vector>

extern const float PI;

class Planet {
   public:
    glm::vec3 position;
    float mass;
    float radius;

    glm::vec3 velocity;

    // Planet();
    Planet(glm::vec3 position, glm::vec3 velocity,
               float radius = 1.0f, float mass = 500.0f);

    void render(const Shader&);
    void update(const std::vector<Planet>& other_planets, float delta_time);

   private:
    // Physical Attributes
    float orbit_radius;
    float orbit_speed;
    float angular_speed = glm::radians(-50.0f);

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

    // Vertex Data
    GLuint shader_ID;
    GLuint VAO;
    std::vector<GLuint> indices;
    std::vector<GLuint> line_indices;

    unsigned int latCount = 36;
    unsigned int longCount = 18;

    void initVertexData();
};

#endif