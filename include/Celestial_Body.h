#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include "Renderer/Shader.h"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/trigonometric.hpp>
#include <vector>

extern const float PI;

class Body {
   public:
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;

    Body(glm::vec3 position, glm::vec3 velocity, float mass)
        : position(position), velocity(velocity), mass(mass) {
        initOrbitVertexData();
    }

    // void predictPositions(std::vector<GLfloat>& positionVector,
    //                       const std::vector<Body*>& other_bodies, glm::vec3
    //                       position, glm::vec3 velocity, float timestep, unsigned
    //                       int steps);

    std::vector<GLfloat> predictPositions(const Body& me,
                                            const std::vector<Body*>& others,
                                            float timestep, unsigned int steps);
    void drawOrbit(Shader& shader);
    void updateOrbitVertexData();

   protected:
    GLuint orbitVAO, orbitVBO, orbitEBO;

    std::vector<GLfloat> orbitVertices;
    std::vector<GLuint> orbitIndices;

    void initOrbitVertexData();
};

class Planet : public Body {
   public:
    // Planet();
    Planet(glm::vec3 position, glm::vec3 velocity, float mass = 500.0f,
           float radius = 1.0f);

    void render(const Shader& shader);
    void update(const std::vector<Body*>& other_bodies, float delta_time);

   private:
    // Physical Attributes
    float radius;
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

class Star : public Body {
   public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

    Star(glm::vec3 position, glm::vec3 velocity, float mass, float radius);

    void render(const Shader& shader);
    void update(const std::vector<Body*>& other_bodies, float delta_time);

   private:
    float radius;
    float angular_speed = glm::radians(-50.0f);

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