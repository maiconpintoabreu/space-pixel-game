#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <map>
#include <memory>
#include "physics_object.h"
#include "enums.h"
#include "global.h"

struct PhysicsBody
{
    Vector2 position{};
    Vector2 center{};
    float rotation = 0;
    float rotation_torque = 0;
    float speed_limit = 0;
    float deceleration_multiplier = 0;
    float rotation_speed_limit = 0;
    Vector2 velocity{};
    ObjectShape collision = ObjectShape::Circle;
    ObjectType type = ObjectType::UNKNOWN_TYPE;
    float width = 0.0f;
    float height = 0.0f;
    bool is_alive = false;
    bool is_on_screen = false;
    bool is_collision_enabled = false;
    bool is_accelerating = false;
    bool is_applying_torque = false;
    bool is_rotating_left = false;
    bool is_rotating_right = false;
    std::weak_ptr<PhysicsObject> game_object;
};

class PhysicsSystem
{
private:
    int current_object_id = -1;

    float m_gravity_x;
    float m_gravity_y;
    // unique player for now
    int player_id;
    std::vector<PhysicsBody> physics_body_list;

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
    inline int CreatePhysicsObject(PhysicsBody body)
    {
        int index_saved = static_cast<int>(physics_body_list.size());
        for (int i=0;i<index_saved;i++){
            if(!physics_body_list[i].is_alive){
                physics_body_list[i] = body;
                if(body.type == ObjectType::PLAYER_TYPE){
                    player_id = i;
                }
                return i;
            }
        }
        physics_body_list.push_back(body);
        if(body.type == ObjectType::PLAYER_TYPE){
            player_id = index_saved;
        }
        return index_saved;
    }

    static PhysicsSystem &GetInstance(float gravity_x = 0.0f, float gravity_y = 0.0f)
    {
        static PhysicsSystem instance(gravity_x, gravity_y);
        return instance;
    }

    inline PhysicsBody& GetPhysicsObject(int id)
    {
        return physics_body_list[id];
    }

    // Delete copy constructor and assignment operator
    PhysicsSystem(const PhysicsSystem &) = delete;
    PhysicsSystem &operator=(const PhysicsSystem &) = delete;

    inline void FixUpdate(float delta_time, Vector2 camera_with_offset)
    {
        for (PhysicsBody& body : physics_body_list)
        {
            if (body.is_alive)
            {
                Move(delta_time, body);
                body.velocity.y += m_gravity_y * delta_time;
                body.velocity.x += m_gravity_x * delta_time;
                body.is_on_screen = IsPositionOnScreen(body.position, camera_with_offset);
                if (body.is_on_screen)
                {
                    body.is_collision_enabled = true;
                    if(body.type == ObjectType::ASTEROID_TYPE){
                        CheckAstronomicalObjectCollisions(body);
                    }else if(body.type == ObjectType::BULLET_TYPE){
                        CheckBulletCollisions(body);
                    }
                }
                else
                {
                    body.is_collision_enabled = false;
                }
            }
        }
    }
    inline void Move(float delta_time, PhysicsBody& body){
        
        // ensure our angle is between -180 and +180
        if (body.rotation > 180.0f)
            body.rotation -= 360.0f;

        if (body.rotation < -180.0f)
            body.rotation += 360.0f;

        // Limit speed
        if (Vector2Length(body.velocity) > body.speed_limit)
        {
            body.velocity = Vector2Normalize(body.velocity);
            body.velocity = Vector2Scale(body.velocity, body.speed_limit);
        }

        // Update position if velocity is different then 0
        if (Vector2Length(body.velocity) > 0.0f)
        {
            // Friction
            body.velocity = Vector2Scale(body.velocity, 1 - body.deceleration_multiplier * delta_time);
            body.position = Vector2Add(body.position, Vector2Scale(body.velocity, delta_time));
        }else{
            body.is_accelerating = false;
        }
        // adjust rotation_torque to rotation_speed_limit
        if (body.rotation_torque > body.rotation_speed_limit)
        {
            body.rotation_torque = body.rotation_speed_limit;
        }
        else if (body.rotation_torque < -body.rotation_speed_limit)
        {
            body.rotation_torque = -body.rotation_speed_limit;
        }
        // Update rotation if rotational_velocity is different then 0
        if (body.rotation_torque != 0.0f)
        {
            body.rotation += body.rotation_torque * delta_time;
            // Update rotational velocity
            if (!body.is_applying_torque)
            {
                body.rotation_torque = body.rotation_torque * (1 - body.deceleration_multiplier * delta_time);
            }
            body.is_applying_torque = false;
        }
        // // check if still colliding
        // for (int i = body.colliding_objects.size() - 1; i >= 0; i--)
        // {
        //     if (auto shared_object = body.colliding_objects[i].lock())
        //     {
        //         if (!body.IsColliding(shared_object))
        //         {
        //             body.ExitCollision(shared_object);
        //         }
        //     }
        //     else
        //     {
        //         body.colliding_objects.erase(body.colliding_objects.begin() + i);
        //         if (colliding_objects.size() == 0)
        //             body.is_colliding = false;
        //     }
        // }
    }
    inline void RemoveObject(int object_id)
    {
        if (object_id < 0)
            return;
        PhysicsBody& body = physics_body_list[object_id];
        if (!body.is_alive)
            return;
        body.is_alive = false;
        body.game_object.reset();
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
        PhysicsBody& body = physics_body_list[object_id];
        if (!body.is_alive)
            return;
        Vector2 direction = {sinf(body.rotation * DEG2RAD), -cosf(body.rotation * DEG2RAD)};
        body.velocity = Vector2Scale(direction, force);
        body.is_accelerating = true;
    }
    inline void ApplyForce(int object_id, float force, Vector2 direction)
    {
        if (object_id < 0)
            return;
        PhysicsBody& body = physics_body_list[object_id];
        if (!body.is_alive)
            return;
        // move forward based on rotation angle
        body.velocity = Vector2Scale(direction, force);
        body.is_accelerating = true;
    }
    inline void ApplyTorque(int object_id, float torque)
    {
        if (object_id < 0)
            return;
        PhysicsBody& body = physics_body_list[object_id];
        if (!body.is_alive)
            return;
        body.rotation_torque += torque;
        body.is_applying_torque = true;
        body.is_rotating_left = torque < 0;
        body.is_rotating_right = torque > 0;
    }

    // Destructor
    ~PhysicsSystem()
    {
        TraceLog(LOG_INFO, "PhysicsSystem destroyed");
    }
    void Unload()
    {
        physics_body_list.clear();
    }

private:
    // Private constructor
    PhysicsSystem(float gravity_x = 0.0f, float gravity_y = 0.0f)
        : m_gravity_x(gravity_x), m_gravity_y(gravity_y) {}

    inline void CheckBulletCollisions(PhysicsBody& bullet)
    {
        for (PhysicsBody& body : physics_body_list)
        {
            if (body.is_alive && body.type == ObjectType::ASTEROID_TYPE)
            {
                CheckCollision(bullet, body);
            }
        }
    }
    inline void CheckAstronomicalObjectCollisions(PhysicsBody& astronomical_object)
    {
        if (astronomical_object.is_alive)
        {
            if (physics_body_list[player_id].is_alive)
            {
                CheckCollision(astronomical_object, physics_body_list[player_id]);
            }
        }
    }
      inline virtual bool CheckCollision(PhysicsBody& source, PhysicsBody& dest)
    {
        if (!source.is_alive || dest.is_alive)
            return false;
        bool is_colliding = false;
        Vector2 temp_position = source.position + source.center;
        Vector2 other_position = dest.position + dest.center;
        if (source.collision == ObjectShape::Circle)
        {
            if (dest.collision == ObjectShape::Circle)
            {
                is_colliding = CheckCollisionCircles(temp_position, source.width / 2, other_position, dest.width / 2);
            }
            else if (dest.collision == ObjectShape::Rectangle)
            {
                is_colliding = CheckCollisionCircleRec(temp_position, source.width / 2, Rectangle({dest.position.x, dest.position.y, dest.width, dest.height}));
            }
        }
        if (source.collision == ObjectShape::Rectangle)
        {
            if (dest.collision == ObjectShape::Circle)
            {
                is_colliding = CheckCollisionCircleRec(temp_position, source.width / 2, Rectangle({dest.position.x, dest.position.y, dest.width, dest.height}));
            }
            else if (dest.collision == ObjectShape::Rectangle)
            {
                is_colliding = CheckCollisionRecs(Rectangle({source.position.x, source.position.y, source.width, source.height}), Rectangle({dest.position.x, dest.position.y, dest.width, (float)dest.height}));
            }
        }
        if (is_colliding){
            if(auto shared_game_object = source.game_object.lock()){
                shared_game_object->EnterCollision(dest.game_object.lock());
            }
        }
        return is_colliding;
    }
};

#endif // PHYSICS_SYSTEM_H