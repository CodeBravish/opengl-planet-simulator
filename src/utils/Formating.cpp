#include "utils/Formating.h"

std::string stringVec3(const glm::vec3& vector) {
    return static_cast<std::string>(std::to_string(vector.x) + ", " +
                                    std::to_string(vector.y) + ", " +
                                    std::to_string(vector.z));
}