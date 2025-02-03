#include "physics_object.h"
#include "physics_system.h"

int PhysicsObject::CreatePhysicsId(std::shared_ptr<PhysicsObject> shared_physic_object) {
    PhysicsBody body = PhysicsBody();
    body.is_alive = true;
    body.type = shared_physic_object->object_type;
    body.center = shared_physic_object->center;
    body.position = shared_physic_object->position;
    body.rotation = shared_physic_object->rotation;
    body.deceleration_multiplier = shared_physic_object->deceleration_multiplier;
    body.collision = shared_physic_object->shape;
    body.game_object = shared_physic_object;
    body.speed_limit = shared_physic_object->speed_limit;
    body.rotation_speed_limit = shared_physic_object->rotation_speed_limit;
    body.width = shared_physic_object->width;
    body.height = shared_physic_object->height;
    body.rotation_torque = shared_physic_object->rotation_torque;

    return PhysicsSystem::GetInstance().CreatePhysicsObject(body);
}