#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "dynamic_body.h"
#include "bullet.h"
#include <vector>
#include <memory>

// Player will control a spaceship
class Player : public DynamicBody
{
private:
    float score = 0.0f;
    bool is_alive = true;
public:
    // Attributes
    float health = 100.0f;
    float max_health = 100.0f;

    float energy = 100.0f;
    float max_energy = 100.0f;

    Texture2D spaceship;
    // Origin
    Vector2 origin;
    bool is_rotating = false;

    // Direction
    Vector2 direction = {0, 0};

    Shader propeller_shader;

    // Gun Attributes
    float gun_range = 200.0f;
    float gun_damage = 10.0f;
    float gun_cooldown = 0.5f;
    float gun_cooldown_time = 0.5f;
    Vector2 gun_socket_left = {0, 0};
    Vector2 gun_socket_right = {0, 0};
    Vector2 gun_position = {0, 0};
    bool is_shooting = false;
    bool is_gun_ready = true;
    std::vector<std::weak_ptr<Bullet>> bullets;
public:
    static std::shared_ptr<Player> Create(){
        std::shared_ptr<Player> obj = std::make_shared<Player>();
        obj->physics_id = PhysicsObject::CreatePhysicsId(obj);
        TraceLog(LOG_INFO, "Object of type Player created");
        return obj;
    }
    Player();
    ~Player()
    {
        if (IsWindowReady() == false)
            return;
        if (spaceship.id > 0)
        {
            UnloadTexture(spaceship);
        }
        bullets.clear();
        TraceLog(LOG_INFO, "Player destroyed");
    }
    void Update(float delta_time) override;
    void FixUpdate(float delta_time) override;
    void Render() override;

    void TurnLeft();
    void TurnRight();
    void Accelerate();
    void Decelerate();

    void Shoot();
    void TakeDamage(float damage, Vector2 point) override;

    float GetRotation() const { return rotation; }
    float GetHealth() const { return health; }
    float GetEnergy() const { return energy; }
    float GetMaxHealth() const { return max_health; }
    float GetMaxEnergy() const { return max_energy; }
    float GetPercentHealth() const { return health / max_health; }
    float GetPercentEnergy() const { return energy / max_energy; }

    void AddScore(float in_score)
    {
        score = in_score;
    }
    float GetScore()
    {
        return score;
    }
    bool IsAlive() {
        return is_alive;
    }
};

#endif // PLAYER_H
