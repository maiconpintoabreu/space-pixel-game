#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "player.h"
#include "input_manager.h"
#include "star_builder.h"
#include "astronomical_object.h"
#include "planet.h"

#include "physics_system.h"
#include "physics_object.h"

class GameManager
{
private:
    bool is_debug = false;
    int frameCounter = 0;
    std::string map_name = "01";
    Camera2D camera;
    std::shared_ptr<Player> player;
    StarBuilder *star_builder = nullptr;
    InputManager *input_manager = nullptr;
    std::shared_ptr<Planet> planet;
    std::vector<std::shared_ptr<PhysicsObject>> physic_objects;

    float asteriod_cooldown = 0.0f;
    float asteriod_cooldown_time = 1.0f;
    bool is_menu = true;
    // menu position stars
    std::vector<Vector2> menu_stars;

public:
    GameManager();
    ~GameManager()
    {
        if (star_builder != nullptr)
        {
            delete star_builder;
        }
        if (input_manager != nullptr)
        {
            delete input_manager;
        }
        physic_objects.clear();
        player.reset();
        PhysicsSystem::GetInstance().Unload();
        TraceLog(LOG_INFO, "GameManager destroyed");
    }

private:
    bool isGameOver_;
    int score;
    void RelocateOriginBasedOnPlayerPosition();
    bool LoadMap();
    // draw buttoms for a menu using Rectangle
    int MenuButtom(Rectangle buttom, const char *buttom_text);

    // Spawn asteroid for testing
    void SpawnAsteroid(float delta_time);

public:
    int getScore() { return score; }
    void Update(float delta_time);
    void FixUpdate(float delta_time);
    void Render();
    bool isGameOver();
};

#endif // GAME_MANAGER_H