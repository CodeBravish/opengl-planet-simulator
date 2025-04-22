#version 330 core
in vec3 FragPos;

uniform vec3 gridColor;
uniform vec3 gridSize;
uniform vec3 cameraPos;

out vec4 FragColor;

void main() {
    float fragDistance = length(cameraPos - FragPos);

    FragColor = vec4(gridColor, 1.0);
}