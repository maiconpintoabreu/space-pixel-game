#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <vector>
#include <memory>
#include "enums.h"

enum class ObjectShape
{
    Circle,
    Rectangle,
    Triangle,
    Lines
};
class PhysicsObject : public std::enable_shared_from_this<PhysicsObject> 
{
private:
    // objects in collision
    std::vector<std::weak_ptr<PhysicsObject>> colliding_objects;

public:
    int physics_id = -1;

    // Properties
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};
    int id = -1;
    float speed_limit = 100.0f;
    float rotation_speed = 0.0f;
    float rotation_speed_limit = 200.0f;
    float width = 0.0f;
    float height = 0.0f;
    Vector2 center = {0, 0};
    float rotation = 0.0f;
    float rotation_torque = 0.0f;
    float deceleration_multiplier = 0.0f;
    float mass = 1.0f;
    bool is_applying_torque = false;
    bool is_collision_enabled = false;

    ObjectShape shape = ObjectShape::Circle;
    bool is_static = false;
    bool is_colliding = false;
    ObjectType object_type;
    bool is_on_screen = false;
    bool is_alive = true; // TODO: change it
    
    bool is_rotating_left = false;
    bool is_rotating_right = false;
    bool is_accelerating = false;
public:
    PhysicsObject() = default;
    static std::shared_ptr<PhysicsObject> Create(){
        std::shared_ptr<PhysicsObject> obj = std::make_shared<PhysicsObject>();
        obj->physics_id = CreatePhysicsId(obj);
        TraceLog(LOG_INFO, "Object of type PhysicsObject created");
        return obj;
    }

    inline virtual ~PhysicsObject()
    {
        colliding_objects.clear();

        TraceLog(LOG_INFO, TextFormat("PhysicsObject destroyed"));
    };

    inline virtual void Update(float delta_time)
    {
        // do nothing yet
    }
    inline virtual void FixUpdate(float delta_time)
    {
        if (is_static)
            return;
        // ensure our angle is between -180 and +180
        if (rotation > 180.0f)
            rotation -= 360.0f;

        if (rotation < -180.0f)
            rotation += 360.0f;

        // Limit speed
        if (Vector2Length(velocity) > speed_limit)
        {
            velocity = Vector2Normalize(velocity);
            velocity = Vector2Scale(velocity, speed_limit);
        }

        // Update position if velocity is different then 0
        if (Vector2Length(velocity) > 0.0f)
        {
            // Friction
            velocity = Vector2Scale(velocity, 1 - deceleration_multiplier * delta_time);
            position = Vector2Add(position, Vector2Scale(velocity, delta_time));
        }else{
            is_accelerating = false;
        }
        // adjust rotation_torque to rotation_speed_limit
        if (rotation_torque > rotation_speed_limit)
        {
            rotation_torque = rotation_speed_limit;
        }
        else if (rotation_torque < -rotation_speed_limit)
        {
            rotation_torque = -rotation_speed_limit;
        }
        // Update rotation if rotational_velocity is different then 0
        if (rotation_torque != 0.0f)
        {
            rotation += rotation_torque * delta_time;
            // Update rotational velocity
            if (!is_applying_torque)
            {
                rotation_torque = rotation_torque * (1 - deceleration_multiplier * delta_time);
            }
            is_applying_torque = false;
        }
        // check if still colliding
        for (int i = colliding_objects.size() - 1; i >= 0; i--)
        {
            if (auto shared_object = colliding_objects[i].lock())
            {
                if (!IsColliding(shared_object))
                {
                    ExitCollision(shared_object);
                }
            }
            else
            {
                colliding_objects.erase(colliding_objects.begin() + i);
                if (colliding_objects.size() == 0)
                    is_colliding = false;
            }
        }
    }

    inline virtual bool CheckCollision(std::shared_ptr<PhysicsObject> other)
    {
        if (!other)
            return false;
        bool is_local_colliding = false;
        Vector2 temp_position = position + center;
        Vector2 other_position = other->position + other->center;
        if (shape == ObjectShape::Circle)
        {
            if (other->shape == ObjectShape::Circle)
            {
                is_local_colliding = CheckCollisionCircles(temp_position, width / 2, other_position, other->width / 2);
            }
            else if (other->shape == ObjectShape::Rectangle)
            {
                is_local_colliding = CheckCollisionCircleRec(temp_position, width / 2, Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
        }
        if (shape == ObjectShape::Rectangle)
        {
            if (other->shape == ObjectShape::Circle)
            {
                is_local_colliding = CheckCollisionCircleRec(temp_position, width / 2, Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
            else if (other->shape == ObjectShape::Rectangle)
            {
                is_local_colliding = CheckCollisionRecs(Rectangle({position.x, position.y, width, height}), Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
        }
        if (is_local_colliding)
            EnterCollision(other);
        return is_local_colliding;
    }

    inline virtual void EnterCollision(std::shared_ptr<PhysicsObject> other)
    {
        if (!other)
            return;
        is_colliding = true;
        for (auto &object : colliding_objects)
        {
            if (auto shared_object = object.lock())
            {
                if (shared_object->id == other->id)
                {
                    return;
                }
            }
        }
        // add to colliding objects
        colliding_objects.push_back(other);
        EnterCollision(shared_from_this());
    }

    inline virtual void ExitCollision(std::shared_ptr<PhysicsObject> other)
    {
        // remove from colliding objects
        for (size_t  i = 0; i < colliding_objects.size(); ++i)
        {
            if (colliding_objects[i].lock() == other)
            {
                colliding_objects[i].reset();
            }
            colliding_objects.erase(colliding_objects.begin() + i);
            break;
        }
        // ExitCollision(other);
        if (colliding_objects.size() == 0)
            is_colliding = false;
    }

    inline void SetPosition(float x, float y)
    {
        position.x = x;
        position.y = y;
    }

    inline void SetVelocity(float vx, float vy)
    {
        velocity.x = vx;
        velocity.y = vy;
    }

    inline void SetRotation(float angle)
    {
        rotation = angle;
    }

    inline void SetShape(ObjectShape s)
    {
        shape = s;
    }
    // debug render shape of object and if is colliding
    inline virtual void Render()
    {
        if (shape == ObjectShape::Rectangle)
        {
            if (is_colliding)
            {
                DrawRectangleRec(Rectangle({position.x, position.y, width, height}), RED);
            }
            else
            {
                DrawRectangleRec(Rectangle({position.x, position.y, width, height}), {0, 255, 0, 100});
            }
        }
        else if (shape == ObjectShape::Circle)
        {
            if (is_colliding)
            {
                DrawCircleV(position, width / 2, RED);
            }
            else
            {
                DrawCircleV(position, width / 2, {0, 255, 0, 100});
            }
        }
    }
    virtual void TakeDamage(float damage, Vector2 point){};
protected:
    static int CreatePhysicsId(std::shared_ptr<PhysicsObject> shared_physic_object);
private:
    // Check if is current colliding
    inline bool IsColliding(std::shared_ptr<PhysicsObject> other)
    {
        if (!other)
            return false;
        bool is_local_colliding = false;
        Vector2 temp_position = position + center;
        Vector2 other_position = other->position + other->center;
        if (shape == ObjectShape::Circle)
        {
            if (other->shape == ObjectShape::Circle)
            {
                is_local_colliding = CheckCollisionCircles(temp_position, width / 2, other_position, other->width / 2);
            }
            else if (other->shape == ObjectShape::Rectangle)
            {
                is_local_colliding = CheckCollisionCircleRec(temp_position, width / 2, Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
        }
        if (shape == ObjectShape::Rectangle)
        {
            if (other->shape == ObjectShape::Circle)
            {
                is_local_colliding = CheckCollisionCircleRec(temp_position, width / 2, Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
            else if (other->shape == ObjectShape::Rectangle)
            {
                is_local_colliding = CheckCollisionRecs(Rectangle({position.x, position.y, width, height}), Rectangle({other->position.x, other->position.y, other->width, other->height}));
            }
        }
        if (is_local_colliding)
            EnterCollision(other);
        return is_local_colliding;
    }
    bool CheckCollisionPolygons(const std::vector<Vector2> &poly1, const std::vector<Vector2> &poly2)
    {
        if (poly1.size() < 3 || poly2.size() < 3)
        {
            return false; // Polygons must have at least 3 vertices
        }
        std::vector<Vector2> allVertices;
        allVertices.insert(allVertices.end(), poly1.begin(), poly1.end());
        allVertices.insert(allVertices.end(), poly2.begin(), poly2.end());

        auto getNormals = [](const std::vector<Vector2> &poly)
        {
            std::vector<Vector2> normals;
            for (size_t  i = 0; i < poly.size(); ++i)
            {
                Vector2 edge = poly[(i + 1) % poly.size()] - poly[i];
                normals.push_back({-edge.y, edge.x}); // 90-degree rotation for normal
            }
            return normals;
        };

        std::vector<Vector2> normals1 = getNormals(poly1);
        std::vector<Vector2> normals2 = getNormals(poly2);

        auto projectPolygon = [](const std::vector<Vector2> &poly, const Vector2 &axis)
        {
            float min = 1e9f, max = -1e9f;
            for (const auto &vertex : poly)
            {
                float projection = Vector2DotProduct(vertex, axis);
                min = std::min(min, projection);
                max = std::max(max, projection);
            }
            return std::make_pair(min, max);
        };

        for (const auto &normal : normals1)
        {
            auto [minA, maxA] = projectPolygon(poly1, normal);
            auto [minB, maxB] = projectPolygon(poly2, normal);
            if (!Intersect(minA, maxA, minB, maxB))
            {
                return false;
            }
        }

        for (const auto &normal : normals2)
        {
            auto [minA, maxA] = projectPolygon(poly1, normal);
            auto [minB, maxB] = projectPolygon(poly2, normal);
            if (!Intersect(minA, maxA, minB, maxB))
            {
                return false;
            }
        }

        return true;
    }

    bool Intersect(float minA, float maxA, float minB, float maxB)
    {
        return !(maxB < minA || maxA < minB);
    }
};

#endif // PHYSICS_OBJECT_H