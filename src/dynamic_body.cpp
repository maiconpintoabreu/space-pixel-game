#include "dynamic_body.h"
#include "physics_system.h"

// Apply force to move forward
void DynamicBody::ApplyForce(float force)
{
    if (physics_id < 0)
        return;
    // move forward based on rotation angle
    PhysicsSystem::GetInstance().ApplyForce(physics_id, force);
};
// Apply force to move forward
void DynamicBody::ApplyForceDirected(float force, Vector2 direction)
{
    if (physics_id < 0)
        return;
    // move forward based on rotation angle 
    PhysicsSystem::GetInstance().ApplyForce(physics_id, force, direction);
};

// Apply force to move backward
void DynamicBody::ApplyTorque(float torque)
{
    if (physics_id < 0)
        return;
    PhysicsSystem::GetInstance().ApplyTorque(physics_id, torque);
};
