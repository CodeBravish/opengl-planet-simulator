#version 330 core
in vec3 FragPos;

uniform vec3 gridColor;
uniform float mapSize;
uniform vec3 cameraPos;

out vec4 FragColor;

void main() {
    float fragDistance = length(vec3(cameraPos.x, 0.0, cameraPos.z) - FragPos);

    FragColor = vec4(gridColor, max(1.0 - fragDistance / (mapSize/2), 0.0));
}