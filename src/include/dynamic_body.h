#ifndef DYNAMIC_BODY_H
#define DYNAMIC_BODY_H

#include <raylib.h>
#include <raymath.h>
#include "physics_object.h"

class DynamicBody : public PhysicsObject
{
public:
    // Constructor
    DynamicBody()
    {

    }

    // Apply force to move forward
    virtual void ApplyForce(float force);
    // Apply force to move forward
    virtual void ApplyForceDirected(float force, Vector2 direction);

    // Apply force to move backward
    virtual void ApplyTorque(float torque);

    Vector2 GetPosition()
    {
        return position;
    };

    void FixUpdate(float delta_time) override
    {
        // Call the base class version
        PhysicsObject::FixUpdate(delta_time);
    }

    void Render() override
    {
        // Call the base class version
        PhysicsObject::Render();
    }
    void EnterCollision(std::shared_ptr<PhysicsObject> other) override
    {
        // Call the base class version
        PhysicsObject::EnterCollision(other);
    }
    void ExitCollision(std::shared_ptr<PhysicsObject> other) override
    {
        // Call the base class version
        PhysicsObject::ExitCollision(other);
    }

protected:
    Vector3 collider;
    // Rotation speed
    float rotation_speed = 200.0f;
    float speed = 100.0f;

};

#endif // DYNAMIC_BODY_H