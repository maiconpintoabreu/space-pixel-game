#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <algorithm> // for std::remove
#include <map>
#include <memory>
#include "physics_object.h"
#include "enums.h"
#include "global.h"

class PhysicsSystem
{
private:
    int current_object_id = -1;

    float m_gravity_x;
    float m_gravity_y;
    // unique player for now
    std::shared_ptr<PhysicsObject> player;
    std::vector<std::shared_ptr<PhysicsObject>> physic_objects;
    std::map<int, std::weak_ptr<PhysicsObject>> by_id_physic_objects;


    inline bool IsPositionOnScreen(Vector2 world_position, Vector2 camera_position)
    {
        if (world_position.x >= camera_position.x && world_position.x <= camera_position.x + virtual_screen_width &&
            world_position.y >= camera_position.y && world_position.y <= camera_position.y + virtual_screen_height)
        {
            return true; // Position is on screen
        }
        else
        {
            return false; // Position is off screen
        }
    }
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
            physic_objects.push_back(shared_physic_object);
            break;
        case ObjectType::BULLET_TYPE:
            physic_objects.push_back(shared_physic_object);
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
    inline std::vector<std::shared_ptr<PhysicsObject>> GetPhysiscsObjectList()
    {
        return physic_objects;
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

    inline void FixUpdate(float delta_time, Vector2 camera_with_offset)
    {
        if (player)
        {
            player->velocity.y += m_gravity_y * delta_time;
            player->velocity.x += m_gravity_x * delta_time;
            player->FixUpdate(delta_time);
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
        for (const auto &obj : physic_objects)
        {
            if (obj)
            {
                obj->velocity.y += m_gravity_y * delta_time;
                obj->velocity.x += m_gravity_x * delta_time;
                obj->FixUpdate(delta_time);
                obj->is_on_screen = IsPositionOnScreen(obj->position, camera_with_offset);
                if (obj->is_on_screen)
                {
                    obj->is_collision_enabled = true;
                }
                else
                {
                    obj->is_collision_enabled = false;
                }
                if(obj->object_type == ObjectType::ASTEROID_TYPE){
                    CheckAstronomicalObjectCollisions(obj);
                }else if(obj->object_type == ObjectType::BULLET_TYPE){
                    CheckBulletCollisions(obj);
                }
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
        int index_to_remove = -1;
        int last_index = -1;
        if (auto shared_physic_object = it->second.lock())
        {
            switch (shared_physic_object->object_type)
            {
            case ObjectType::PLAYER_TYPE:
                player.reset();
                break;
            case ObjectType::ASTEROID_TYPE:
                // remove from colliding objects
                for (size_t  i = 0; i < physic_objects.size(); ++i)
                {
                    if(auto shared_physics_object = physic_objects[i]){
                        if (shared_physics_object->id == object_id)
                        {
                            index_to_remove = i;
                            break;
                        }
                    }
                }
                if(index_to_remove < 0) return;
                last_index = static_cast<int>(physic_objects.size())-1;
                if(index_to_remove > last_index) return;
                physic_objects[index_to_remove].reset();
                if(index_to_remove != 0){
                    physic_objects[index_to_remove] = physic_objects[last_index];
                }
                physic_objects.pop_back();
                break;
            case ObjectType::BULLET_TYPE:
                // remove from colliding objects
                for (size_t  i = 0; i < physic_objects.size(); ++i)
                {
                    if(auto shared_physics_object = physic_objects[i]){
                        if (shared_physics_object->id == object_id)
                        {
                            index_to_remove = i;
                            break;
                        }
                    }
                }
                TraceLog(LOG_DEBUG, TextFormat("index removed: %i", index_to_remove));
                if(index_to_remove < 0) return;
                last_index = static_cast<int>(physic_objects.size())-1;
                if(index_to_remove > last_index) return;
                physic_objects[index_to_remove].reset();
                if(index_to_remove != 0){
                    physic_objects[index_to_remove] = physic_objects[last_index];
                }
                physic_objects.pop_back();
                TraceLog(LOG_DEBUG, TextFormat("index removed: %i", index_to_remove));
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
        for (size_t  i = 0; i < physic_objects.size(); ++i)
        {
            physic_objects[i].reset();
        }
        physic_objects.clear();
        player.reset();
        by_id_physic_objects.clear();
    }

private:
    // Private constructor
    inline PhysicsSystem(float gravity_x = 0.0f, float gravity_y = 0.0f)
        : m_gravity_x(gravity_x), m_gravity_y(gravity_y) {}

    // inline void CheckCollisions() {
    //     for (int i = 0; i < physic_objects.size(); ++i) {
    //         if(physic_objects[i]->is_collision_enabled){
    //             for (int j = i + 1; j < physic_objects.size(); ++j) {
    //                 if (physic_objects[i] && physic_objects[j] && physic_objects[j]->is_collision_enabled) {
    //                     physic_objects[i]->CheckCollision(physic_objects[j]);
    //                 }
    //             }
    //         }
    //     }
    // }
    inline void CheckBulletCollisions(std::shared_ptr<PhysicsObject> bullet)
    {
        for (const auto &obj : physic_objects)
        {
            if (obj && obj->is_alive && obj->object_type == ObjectType::ASTEROID_TYPE)
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