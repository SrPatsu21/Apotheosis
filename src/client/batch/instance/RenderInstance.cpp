#include "RenderInstance.hpp"

#include <glm/gtc/matrix_transform.hpp>

RenderInstance::RenderInstance(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
) :
    position(position),
    rotation(rotation),
    scale(scale)
{ }

void RenderInstance::updateModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));

    model = glm::scale(model, scale);

    ownerBatch->getinstancesData()[indexInBatch] = model;
}

RenderInstance::~RenderInstance()
{
    if (ownerBatch)
        ownerBatch->removeInstance(this);
}