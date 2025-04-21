#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <glad/glad.h>
#include <glm/ext/scalar_constants.hpp>
#include <vector>

namespace Geometry {

struct GeometryData {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> indices;
};

inline GeometryData genUVSphere(float radius, unsigned int sectorCount,
                                unsigned int stackCount);

inline GeometryData genUVSphere(float radius, unsigned int sectorCount,
                                unsigned int stackCount) {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> indices;

    const float PI = glm::pi<float>();

    float x, y, z, xy;                            // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;  // vertex normal

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);       // r * cos(u)
        z = radius * sinf(stackAngle);        // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;  // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }

    // std::vector<int> lineIndices;
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

            // // store indices for lines
            // // vertical lines for all stacks, k1 => k2
            // line_indices.push_back(k1);
            // line_indices.push_back(k2);
            // if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            // {
            //     line_indices.push_back(k1);
            //     line_indices.push_back(k1 + 1);
            // }
        }
    }
}
}  // namespace Geometry

#endif  // GEOMETRY_H