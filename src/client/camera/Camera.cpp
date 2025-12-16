// TODO

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 5.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};