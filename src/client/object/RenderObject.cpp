#include "RenderObject.hpp"

#include <glm/gtc/matrix_transform.hpp>

RenderObject::RenderObject(
    std::shared_ptr<Mesh> mesh,
    std::shared_ptr<Material> material,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
    : mesh(std::move(mesh))
    , material(std::move(material))
    , position(position)
    , rotation(rotation)
    , scale(scale)
{
    updateModelMatrix();
}

void RenderObject::updateModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));

    model = glm::scale(model, scale);

    modelMatrix = model;
}
