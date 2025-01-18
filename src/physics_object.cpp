#include "physics_object.h"
#include "physics_system.h"

int PhysicsObject::CreatePhysicsId(std::shared_ptr<PhysicsObject> shared_physic_object) {
    return PhysicsSystem::GetInstance().CreatePhysicsObject(shared_physic_object);
}