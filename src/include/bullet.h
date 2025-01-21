#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "dynamic_body.h"
#include <vector>
#include "physics_system.h"
#include <memory>
#include <queue>
#include "astronomical_object.h"

enum class BulletEventType {
  COLLISION,
  END_OF_LIFE,
  UNKNOWN
};
struct BulletEvent {
  BulletEventType type;
  std::weak_ptr<PhysicsObject> source;
  std::weak_ptr<PhysicsObject> target;
  Vector2 position;
  float timestamp;
};


class Bullet : public DynamicBody
{
private:
    float life_time_base = 1.0f;

public:
    Texture2D bullet_texture;
    bool is_enabled = false;
    bool is_shooting = false;
    float life_time = 10.0f;
    float damage = 100.0f;
    std::weak_ptr<PhysicsObject> owner;
    std::queue<BulletEvent> eventQueue;

public:
    static std::shared_ptr<Bullet> Create(){
        std::shared_ptr<Bullet> obj = std::make_shared<Bullet>();
        obj->physics_id = PhysicsObject::CreatePhysicsId(obj);
        TraceLog(LOG_INFO, "Object of type Bullet created");
        return obj;
    }
    Bullet()
    {
        Image bullet_image = GenImageColor(16, 16, BLANK);
        ImageDrawRectangleV(&bullet_image, {7, 7}, {2, 2}, RED);
        if (IsWindowReady() == true)
        {
            bullet_texture = LoadTextureFromImage(bullet_image);
        }
        else
        {
            bullet_texture = {0};
        }

        deceleration_multiplier = 0.0f;
        UnloadImage(bullet_image);

        object_type = ObjectType::BULLET_TYPE;
        height = 2.0f;
        width = 2.0f;
        center = {bullet_texture.width / 2.0f, bullet_texture.height / 2.0f};
        is_on_screen = true; // remove when I find a way to spawn the bullets on game manager
    };
    ~Bullet()
    {
        if (bullet_texture.id > 0)
        {
            UnloadTexture(bullet_texture);
        }
        TraceLog(LOG_INFO, "Bullet destroyed");
    }

    void Update(float delta_time) override
    {
        if (!is_enabled)
            return;
        // Update life time
        life_time -= delta_time;
        if (life_time <= 0)
        {
            Destroy();
        }
        rotation = rotation;
    }

    void FixUpdate(float delta_time) override
    {
        // Call the base class version
        DynamicBody::FixUpdate(delta_time);
    }

    void Render() override
    {
        if (!IsWindowReady())
            return;
        if (!is_enabled)
            return;
        if (!is_on_screen)
            return;
        // Draw rotated texture
        DrawTexturePro(
            bullet_texture,
            Rectangle({0, 0, static_cast<float>(bullet_texture.width), static_cast<float>(bullet_texture.height)}),                   // Source rectangle (full texture)
            Rectangle({position.x, position.y, static_cast<float>(bullet_texture.width), static_cast<float>(bullet_texture.height)}), // Destination rectangle (position and size)
            {bullet_texture.width / 2.0f, bullet_texture.height / 2.0f},                                                              // Origin of rotation (center of the texture)
            rotation,                                                                                                                 // Rotation in degrees
            WHITE                                                                                                                     // Tint color
        );

        // Update bullet rotation (optional)
        // bulletRotation = atan2f(direction.y, bulletDirection.x) * RAD2DEG;
        // float bullet_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; 
        // float bullet_position[2] = {position.x, position.y};
    }

    void SetEnabled(bool enabled)
    {
        is_enabled = enabled;
        if (enabled)
            life_time = life_time_base;
        is_shooting = false;
    }

    // override
    void EnterCollision(std::shared_ptr<PhysicsObject> object) override
    {
        // Call the base class version
        DynamicBody::EnterCollision(object);
        if (!is_enabled)
            return;
        if (object->object_type == ObjectType::ASTEROID_TYPE)
        {
            if(object->is_alive)
            {
                object->TakeDamage(damage, position);
                if(std::shared_ptr<PhysicsObject> shared_owner = owner.lock())
                {
                    if(shared_owner->object_type == ObjectType::PLAYER_TYPE)
                    {
                        // Trigger a collision event
                        BulletEvent collisionEvent;
                        collisionEvent.type = BulletEventType::COLLISION;
                        collisionEvent.source = shared_from_this();
                        collisionEvent.target = object;
                        collisionEvent.position = position;
                        collisionEvent.timestamp = GetTime(); 
                        eventQueue.push(collisionEvent); 
                    }
                }
            }
        }
    }
    void Destroy()
    {
        PhysicsSystem::GetInstance().RemoveObject(physics_id);
        physics_id = -1; // reset physics id
    }
};

#endif // BULLET_H