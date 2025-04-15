#ifndef PLANET_H
#define PLANET_H

#include "glad/glad.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Planet {
    public:
        vec3 position;
        float mass;
        float radius;

        glm::vec3 velocity;


        // Planet();
        Planet(vec3 position, float radius, float mass);

        void render();

    private:
        // Physical Attributes
        float orbitRadius;
        float orbitSpeed;

        glm::vec3 color = vec3(1.0f, 1.0f, 0.0f);

        // Vertex Data
        GLuint shader_ID;
        GLuint VAO;
        unsigned int latCount = 36;
        unsigned int longCount = 18;

        void initVertexData();
};

#endif