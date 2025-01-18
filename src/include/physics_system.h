#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <algorithm> // for std::remove
#include <map>
#include <memory>
#include "physics_object.h"
#include "enums.h"

class PhysicsSystem
{
private:
    int current_object_id = -1;

    float m_gravity_x;
    float m_gravity_y;
    // unique player for now
    std::shared_ptr<PhysicsObject> player;
    // astronomical objects
    std::vector<std::shared_ptr<PhysicsObject>> physic_astronomical_objects;
    // bullets
    std::vector<std::shared_ptr<PhysicsObject>> physic_bullet_objects;
    std::map<int, std::weak_ptr<PhysicsObject>> by_id_physic_objects;


    inline bool IsPositionOnScreen(Vector2 world_position, Vector2 camera_position)
    {
        if (world_position.x >= camera_position.x && world_position.x <= camera_position.x + GetScreenWidth() &&
            world_position.y >= camera_position.y && world_position.y <= camera_position.y + GetScreenHeight())
        {
            return true; // Position is on screen
        }
        else
        {
            return false; // Position is off screen
        }
    }
public:
    Vector2 camera_position = {0.0f, 0.0f};

public:
    inline int CreatePhysicsObject(std::shared_ptr<PhysicsObject> shared_physic_object)
    {
        int index = ++current_object_id;
        shared_physic_object->id = index;
        by_id_physic_objects[index] = shared_physic_object;
        switch (shared_physic_object->object_type)
        {
        case ObjectType::PLAYER_TYPE:
            player = shared_physic_object;
            break;
        case ObjectType::ASTEROID_TYPE:
            physic_astronomical_objects.push_back(shared_physic_object);
            break;
        case ObjectType::BULLET_TYPE:
            physic_bullet_objects.push_back(shared_physic_object);
            break;
        default:
            TraceLog(LOG_WARNING, "Unknown object type");
            break;
        }
        TraceLog(LOG_INFO, TextFormat("PhysicsObject %i created with id: %i", shared_physic_object->object_type, index));
        return index;
    }

    static PhysicsSystem &GetInstance(float gravity_x = 0.0f, float gravity_y = 0.0f)
    {
        static PhysicsSystem instance(gravity_x, gravity_y);
        return instance;
    }

    inline std::shared_ptr<PhysicsObject> GetPhysicsObject(int id)
    {
        if (auto shared_physic_object = by_id_physic_objects.at(id).lock())
        {
            return shared_physic_object;
        }
        return nullptr;
    }

    // Delete copy constructor and assignment operator
    PhysicsSystem(const PhysicsSystem &) = delete;
    PhysicsSystem &operator=(const PhysicsSystem &) = delete;

    inline void Update(float deltaTime)
    {
    }

    inline void FixUpdate(float deltaTime, Vector2 camera_with_offset)
    {
        if (player)
        {
            player->velocity.y += m_gravity_y * deltaTime;
            player->velocity.x += m_gravity_x * deltaTime;
            player->FixUpdate(deltaTime);
            player->is_on_screen = IsPositionOnScreen(player->position, camera_with_offset);
            if (player->is_on_screen)
            {
                player->is_collision_enabled = true;
            }
            else
            {
                player->is_collision_enabled = false;
            }
        }
        for (const auto &obj : physic_bullet_objects)
        {
            if (obj)
            {
                obj->velocity.y += m_gravity_y * deltaTime;
                obj->velocity.x += m_gravity_x * deltaTime;
                obj->FixUpdate(deltaTime);
                obj->is_on_screen = IsPositionOnScreen(obj->position, camera_with_offset);
                if (obj->is_on_screen)
                {
                    obj->is_collision_enabled = true;
                }
                else
                {
                    obj->is_collision_enabled = false;
                }
                CheckBulletCollisions(obj);
            }
        }
        for (const auto &obj : physic_astronomical_objects)
        {
            if (obj)
            {
                obj->velocity.y += m_gravity_y * deltaTime;
                obj->velocity.x += m_gravity_x * deltaTime;
                obj->FixUpdate(deltaTime);
                obj->is_on_screen = IsPositionOnScreen(obj->position, camera_with_offset);
                if (obj->is_on_screen)
                {
                    obj->is_collision_enabled = true;
                }
                else
                {
                    obj->is_collision_enabled = false;
                }
            }
            CheckAstronomicalObjectCollisions(obj);
        }
    }
    inline void Render()
    {
        for (const auto &obj : physic_astronomical_objects)
        {
            if (obj && obj->is_alive)
            {
                obj->Render();
            }
        }
        for (const auto &obj : physic_bullet_objects)
        {
            if (obj && obj->is_alive)
            {
                obj->Render();
            }
        }
        if (player)
        {
            if (player->is_alive)
            {
                player->Render();
            }
        }
    }
    inline void RemoveObject(int object_id)
    {
        if (object_id < 0)
            return;
        auto it = by_id_physic_objects.find(object_id);
        if (it == by_id_physic_objects.end())
            return;
        if (auto shared_physic_object = it->second.lock())
        {
            switch (shared_physic_object->object_type)
            {
            case ObjectType::PLAYER_TYPE:
                player.reset();
                break;
            case ObjectType::ASTEROID_TYPE:
                physic_astronomical_objects.erase(std::remove(physic_astronomical_objects.begin(), physic_astronomical_objects.end(), shared_physic_object), physic_astronomical_objects.end());
                break;
            case ObjectType::BULLET_TYPE:
                physic_bullet_objects.erase(std::remove(physic_bullet_objects.begin(), physic_bullet_objects.end(), shared_physic_object), physic_bullet_objects.end());
                break;
            default:
                TraceLog(LOG_WARNING, "Unknown object type");
                break;
            }
            by_id_physic_objects.erase(object_id);
        }
    }

    inline void SetGravity(float x, float y)
    {
        m_gravity_x = x;
        m_gravity_y = y;
    }

    inline void ApplyForce(int object_id, float force)
    {
        if (object_id < 0)
            return;
        if (by_id_physic_objects.count(object_id) == 0)
            return;
        const auto &obj = by_id_physic_objects.at(object_id);
        // move forward based on rotation angle
        // TODO: make it to accenerate by time/force using lerp
        if (auto shared_physic_object = obj.lock())
        {
            Vector2 direction = {sinf(shared_physic_object->rotation * DEG2RAD), -cosf(shared_physic_object->rotation * DEG2RAD)};
            shared_physic_object->velocity = Vector2Scale(direction, force);
            shared_physic_object->is_accelerating = true;
        }
    }
    inline void ApplyForce(int object_id, float force, Vector2 direction)
    {
        if (object_id < 0)
            return;
        auto it = by_id_physic_objects.find(object_id);
        if (it == by_id_physic_objects.end())
            return;
        if (auto shared_physic_object = it->second.lock())
        {
            // move forward based on rotation angle
            shared_physic_object->velocity = Vector2Scale(direction, force);
            shared_physic_object->is_accelerating = true;
        }
    }
    inline void ApplyTorque(int object_id, float torque)
    {
        if (object_id < 0)
            return;
        auto it = by_id_physic_objects.find(object_id);
        if (it == by_id_physic_objects.end())
            return;
        if (auto shared_physic_object = it->second.lock())
        {
            shared_physic_object->rotation_torque += torque;
            shared_physic_object->is_applying_torque = true;
            shared_physic_object->is_rotating_left = torque < 0;
            shared_physic_object->is_rotating_right = torque > 0;
        }
    }

    // Destructor
    ~PhysicsSystem()
    {
        TraceLog(LOG_INFO, "PhysicsSystem destroyed");
    }
    void Unload()
    {
        for (int i = 0; i < physic_astronomical_objects.size(); ++i)
        {
            physic_astronomical_objects[i].reset();
        }
        physic_astronomical_objects.clear();
        for (int i = 0; i < physic_bullet_objects.size(); ++i)
        {
            physic_bullet_objects[i].reset();
        }
        physic_bullet_objects.clear();
        player.reset();
        by_id_physic_objects.clear();
    }

private:
    // Private constructor
    inline PhysicsSystem(float gravity_x = 0.0f, float gravity_y = 0.0f)
        : m_gravity_x(gravity_x), m_gravity_y(gravity_y) {}

    // inline void CheckCollisions() {
    //     for (size_t i = 0; i < physic_objects.size(); ++i) {
    //         if(physic_objects[i]->is_collision_enabled){
    //             for (size_t j = i + 1; j < physic_objects.size(); ++j) {
    //                 if (physic_objects[i] && physic_objects[j] && physic_objects[j]->is_collision_enabled) {
    //                     physic_objects[i]->CheckCollision(physic_objects[j]);
    //                 }
    //             }
    //         }
    //     }
    // }
    inline void CheckBulletCollisions(std::shared_ptr<PhysicsObject> bullet)
    {
        for (const auto &obj : physic_astronomical_objects)
        {
            if (obj && obj->is_alive)
            {
                bullet->CheckCollision(obj);
            }
        }
    }
    inline void CheckAstronomicalObjectCollisions(std::shared_ptr<PhysicsObject> astronomical_object)
    {
        if (astronomical_object && astronomical_object->is_alive)
        {
            // for (const auto& other : physic_astronomical_objects) {
            //     if (other && other->is_alive) {
            //         astronomical_object->CheckCollision(other);
            //     }
            // }
            // for (const auto& other : physic_bullet_objects) {
            //     if (other && other->is_alive) {
            //         astronomical_object->CheckCollision(other);
            //     }
            // }
            if (player && player->is_alive)
            {
                astronomical_object->CheckCollision(player);
            }
        }
    }
};

#endif // PHYSICS_SYSTEM_H