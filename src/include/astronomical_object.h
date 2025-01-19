#ifndef ASTRONOMICAL_OBJECT_H
#define ASTRONOMICAL_OBJECT_H

#include "dynamic_body.h"
#include "player.h"
#include <vector>

struct PointForShape
{
    Vector2 position;
    Vector2 direction;
    Color color;
};
struct Shape
{
    std::vector<PointForShape> points;
};

class AstronomicalObject : public DynamicBody
{
private:
    float mass = 0.0f;
    float size = 0.0f;
    int rarity = 0;
    float temperature = 0.0f;
    float life = 100.0f;
    float damage = 0.0f;
    bool is_alive = true;
    Texture2D asteroid_texture;
    float move_by = 4; // TODO: remove this
    Shape asteroid_shape_01 = {
        {{{5, 2 + move_by}, {5, 1}, LIGHTGRAY},
         {{4, 3 + move_by}, {2, 1}, LIGHTGRAY},
         {{5, 3 + move_by}, {5, 1}, GRAY},
         {{10, 3 + move_by}, {1, 1}, LIGHTGRAY},
         {{4, 4 + move_by}, {2, 1}, LIGHTGRAY},
         {{5, 4 + move_by}, {5, 1}, GRAY},
         {{10, 4 + move_by}, {1, 1}, LIGHTGRAY},
         {{5, 5 + move_by}, {5, 1}, LIGHTGRAY}}};
public:
    static std::shared_ptr<AstronomicalObject> Create(ObjectType in_object_type, float in_mass, float in_size, int in_rarity, Vector2 in_position, Vector2 in_speed, float in_speed_limit, float temperature){
        std::shared_ptr<AstronomicalObject> obj = std::make_shared<AstronomicalObject>(in_object_type, in_mass, in_size, in_rarity, in_position, in_speed, in_speed_limit, temperature);
        obj->physics_id = PhysicsObject::CreatePhysicsId(obj);
        TraceLog(LOG_INFO, "Object of type AstronomicalObject created");
        return obj;
    }
    AstronomicalObject(ObjectType in_object_type, float in_mass, float in_size, int in_rarity, Vector2 in_position, Vector2 in_speed, float in_speed_limit, float temperature){
        mass = in_mass;
        size = in_size;
        rarity = in_rarity;
        temperature = temperature;
        position = in_position;
        speed_limit = in_speed_limit;
        life = 100.0f;
        damage = mass * 0.5f;
        is_alive = true;
        deceleration_multiplier = 0.00f;

        Image asteroid_image = GenImageColor(16, 16, BLANK);

        for (size_t i = 0; i < asteroid_shape_01.points.size(); i++)
        {
            ImageDrawRectangleV(&asteroid_image, asteroid_shape_01.points[i].position, asteroid_shape_01.points[i].direction, asteroid_shape_01.points[i].color);
        }
        if (IsWindowReady() == true)
        {
            asteroid_texture = LoadTextureFromImage(asteroid_image);
        }
        else
        {
            asteroid_texture = {0};
        }
        UnloadImage(asteroid_image);
        object_type = in_object_type;
        height = size / 2.0f;
        width = size / 2.0f;
        center = {asteroid_texture.width / 2.0f, asteroid_texture.height / 2.0f};
    }
    // destructor to unload texture
    ~AstronomicalObject()
    {
        PhysicsSystem::GetInstance().RemoveObject(physics_id);
        if (asteroid_texture.id > 0)
        {
            UnloadTexture(asteroid_texture);
        }

        TraceLog(LOG_INFO, "AstronomicalObject destroyed");
    }
    // override
    void EnterCollision(std::shared_ptr<PhysicsObject> object) override
    {
        // Call the base class version
        DynamicBody::EnterCollision(object);
        if (object->object_type == ObjectType::PLAYER_TYPE)
        {
            object->TakeDamage(GetDamage(), position);
            Destroy();
        }
    }

    void Update(float delta_time) override
    {
        if (!is_alive)
            return;
        rotation = rotation;

        UpdateLife(delta_time);
    }

    void FixUpdate(float delta_time) override
    {
        // Call the base class version
        DynamicBody::FixUpdate(delta_time);
    }

    void Render() override
    {
        DynamicBody::Render();
        if (!is_alive)
            return;
        if (!is_on_screen)
            return;
        DrawTexturePro(asteroid_texture, Rectangle({0, 0, static_cast<float>(asteroid_texture.width), static_cast<float>(asteroid_texture.height)}),
                       Rectangle({position.x, position.y, static_cast<float>(asteroid_texture.width), static_cast<float>(asteroid_texture.height)}),
                       {asteroid_texture.width / 2.0f, asteroid_texture.height / 2.0f}, rotation, WHITE);
    }

    float GetDamage() const { return damage; }
    bool IsAlive() const { return is_alive; }
    void TakeDamage(float damage, Vector2 point) override
    {
        life -= damage;
        if (life <= 0)
        {
            Destroy();
        }
    }

private:
    void UpdateLife(float delta_time)
    {
        life -= delta_time * 10.0f * 0.01f;
        if (life <= 0)
        {
            Destroy();
        }
    }
    void Destroy()
    {
        is_alive = false;
        // remove from physic world
        PhysicsSystem::GetInstance().RemoveObject(physics_id);
        physics_id = -1; // reset physics id
    }

public:
    void IsAlive(bool alive) { is_alive = alive; }
    float GetSize() const { return size; }
};

#endif // ASTRONOMICAL_OBJECT_H