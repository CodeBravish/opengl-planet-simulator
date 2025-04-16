#include "Camera.h"

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 position, vec3 front, vec3 up)
    : position(position), front(front), up(up) {}

void Camera::update(const Shader& shader, float delta_time) {
    mat4 projection = mat4(1.0f);

    glm::mat4 view;

    view = glm::lookAt(this->position, this->position + this->center, this->up);

    projection = glm::perspective(
        glm::radians(90.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

    unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
    unsigned int projectionLoc =
        glGetUniformLocation(shader.ID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
}

bool Camera::lookAt(vec3 center) {
    this->center = center;

    if (this->center == vec3(0.0f, 0.0f, 0.0f))
        return false;
    else
        return true;
}