#include "Planet.h"

#include <cmath>
#include <vector>

extern const float PI;

Planet::Planet(vec3 position, float radius, float mass)
    : position(position), radius(radius), mass(mass) {

    }

void Planet::initVertexData() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float radius = 1;
    unsigned int sectorCount = 36;
    unsigned int stackCount = 18;

    float x, y, z, xy;                            // vertex position

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);       // r * cos(u)
        z = radius * sinf(stackAngle);        // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex
        // coords
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;  // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);  // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);  // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    std::vector<int> lineIndices;
    int k1, k2;

    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);  // beginning of current stack
        k2 = k1 + sectorCount + 1;   // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }

    GLuint VBO, EBO;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}