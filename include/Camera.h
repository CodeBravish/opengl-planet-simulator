#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Shader.h"


extern unsigned int screen_width;
extern unsigned int screen_height;
extern unsigned int DefaultShader;



class Camera {
    public:
        Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
        bool lookAt(glm::vec3 center);
        void update(const Shader& shader, float delta_time);
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 center;
        float yaw = 0.0;
        float pitch = 0.0;
    

};

#endif