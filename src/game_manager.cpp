#include "game_manager.h"
#include <string>
#include "enums.h"
#include "global.h"

GameManager::GameManager()
{
    // Initialize game state variables
    isGameOver_ = false;
    score = 1;

    // ... other initializations
    // int frameCounter = 0;
    LoadMap();

    is_debug = true;

    // Initialize menu stars
    for (int i = 0; i < 100; i++)
    {
        menu_stars.push_back({static_cast<float>(GetRandomValue(0, virtual_screen_width)), static_cast<float>(GetRandomValue(0, virtual_screen_height))});
    }
    PhysicsSystem::GetInstance(0.0f, 0.0f); // Initialize physic world
    camera.offset = {0.0f, 0.0f};
    camera.target = {0.0f, 0.0f};
    // Zoom in if the screen increases and zoom out if the screen decreases
    camera.zoom = 1.0f * (virtual_screen_width + virtual_screen_height) / 1000;
    input_manager = new InputManager();
    asteriod_cooldown_time = 0.2f;
    SpawnAsteroid(asteriod_cooldown_time);
    planet = Planet::Create({virtual_screen_width/2.0f, virtual_screen_height/2.0f});
}

void GameManager::Update(float delta_time)
{
    if (player == nullptr)
    {

        // if (IsWindowResized())
        // { // Only update when window is resized
        //     camera.offset = {0.0f, 0.0f};
        //     camera.target = {0.0f, 0.0f};
        //     // Zoom in if the screen increases and zoom out if the screen decreases
        //     // camera.zoom = 1.0f * (virtual_screen_width + virtual_screen_height) / 1000;
        // }
        return;
    }
    // if (IsWindowResized())
    // { // Only update when window is resized
    //     // camera.offset = {virtual_screen_width / 2.0f, virtual_screen_height / 2.0f};
    //     // Zoom in if the screen increases and zoom out if the screen decreases
    //     // camera.zoom = 1.0f * (virtual_screen_width + virtual_screen_height) / 1000;
    //     // if(camera.zoom < 1) camera.zoom = 1.0f;
    //     // star_builder->camera_zoom = camera.zoom;
    // }
    if(!player->IsAlive()){
        // implement game overs screen and destroying physic world, player and star field generator.
        // implement reset game if player choose to try again.
        // the idea is for the player to comeback in the closest space station or beginning position if no space station discovered.
        is_menu = true;
        player.reset();
        camera.target = { 0 };
        camera.offset = { 0 };
        PhysicsSystem::GetInstance().Unload();
        if(star_builder != nullptr){
            delete star_builder;
            star_builder = nullptr;
        }
        return;
    }
    input_manager->Update(delta_time);
    frameCounter++;
    player->Update(delta_time);
    if (player->GetScore() > score)
    {
        score = player->GetScore();
    }
    star_builder->Update(delta_time);
}

void GameManager::RelocateOriginBasedOnPlayerPosition(){
    // MAX 1280 -+
    if(player->position.x < -1280 || player->position.x > 1280){
        // Move all objects based on payer position
        for (std::shared_ptr<PhysicsObject> &obj : physic_objects){
            obj->position.x -= player->position.x;
        }
        star_builder->ReOriginStarsX(player->position.x);
        player->position.x = 0.0f;
    }
    if(player->position.y < -1280 || player->position.y > 1280){
        // Move all objects based on payer position
        for (std::shared_ptr<PhysicsObject> &obj : physic_objects){
            obj->position.y -= player->position.y;
        }
        star_builder->ReOriginStarsY(player->position.y);
        player->position.y = 0.0f;
    }
}

void GameManager::FixUpdate(float delta_time)
{
    if (player == nullptr)  return;
    input_manager->FixUpdate();
    Vector2 camera_with_offset = Vector2Subtract(camera.target, camera.offset);
    PhysicsSystem::GetInstance().FixUpdate(delta_time, camera_with_offset);
    camera.target = player->GetPosition();
    star_builder->FixUpdate(camera.target);
    SpawnAsteroid(delta_time);
    RelocateOriginBasedOnPlayerPosition();
}

void GameManager::Render()
{
    // Check if window is ready
    if (IsWindowReady() == false)
        return;
    // Clear the screen
    ClearBackground(BLACK);
    if (!is_menu)
    {
        BeginMode2D(camera);
            star_builder->Render();
            for(const auto& obj : physic_objects){
                if(obj){
                    PhysicsBody& body = PhysicsSystem::GetInstance().GetPhysicsObject(obj->physics_id);
                    if(body.is_alive && body.is_on_screen){
                        obj->position = body.position;
                        obj->rotation = body.rotation;
                        obj->is_accelerating = body.is_accelerating;
                        obj->is_rotating_left = body.is_rotating_left;
                        obj->is_rotating_right = body.is_rotating_right;
                        obj->Render();
                    }
                }
            }
        EndMode2D();
        input_manager->Render();
    }
    // Draw score and other UI elements
    DrawText(TextFormat("Score: %d", score), 10, 70, 5, GREEN);
    if (is_menu)
    {
        BeginMode2D(camera);
            // Draw random static stars
            for (Vector2 star : menu_stars)
            {
                DrawPixel(star.x, star.y, WHITE);
            }
            planet->Render();
        EndMode2D();
        float size_width = 200.0f;
        float size_height = 50.0f;
        // Add start menu
        if (MenuButtom({static_cast<float>(virtual_screen_width / 2) - size_width / 2, static_cast<float>(virtual_screen_height / 2) - size_height / 1.5f, size_width, size_height}, "Start Game"))
        {
            is_menu = false;

            // Initialize player
            player = Player::Create();
            // player->position = Vector2({0, -10000});
            physic_objects.push_back(player);
            input_manager->SetPlayer(player);
            camera.target = player->GetPosition();
            camera.offset = Vector2({virtual_screen_width / 2.0f, virtual_screen_height / 2.0f});
            // camera.zoom = 1.0f * (virtual_screen_width + virtual_screen_height) / 1000;
            // if(camera.zoom < 1) camera.zoom = 1.0f;
            // camera.rotation = 0.0f;
            star_builder = new StarBuilder(100, camera.target, camera.zoom);
        }
        // Add exit button
        if (MenuButtom({static_cast<float>(virtual_screen_width / 2) - size_width / 2, static_cast<float>(virtual_screen_height / 2) + size_height / 1.5f, size_width, size_height}, "Exit Game"))
        {
            // Exit game
            isGameOver_ = true;
            exit(EXIT_SUCCESS);
        }
    }

    // Draw input manager debug
    if (is_debug)
    {
        // Draw Player position
        if (player) DrawText(TextFormat("Player: %f, %f", player->GetPosition().x, player->GetPosition().y), 10, 100, 5, WHITE);
        // Draw Planet position
        if (planet) DrawText(TextFormat("Planet: %f, %f", planet->GetPosition().x, planet->GetPosition().y), 10, 110, 5, WHITE);
        // Draw screen size
        DrawText(TextFormat("Screen: %i, %i", virtual_screen_width, virtual_screen_height), 10, 130, 5, WHITE);

        // Draw camera zoom
        DrawText(TextFormat("Camera Zoom: %f", camera.zoom), 10, 140, 5, WHITE);

#ifdef __EMSCRIPTEN__
        // Draw monitor size
        DrawText(TextFormat("Monitor: %i, %i", GetMonitorWidth(0), GetMonitorHeight(0)), 10, 150, 5, WHITE);
#else
        // Draw monitor size (This can reduce a lot the FPS)
        //DrawText(TextFormat("Monitor: %i, %i", GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor())), 10, 150, 5, WHITE);
#endif

        // Draw IsWindowFullscreen()
        DrawText(TextFormat("IsWindowFullscreen: %i", IsWindowFullscreen()), 10, 160, 5, WHITE);
        // Draw IsWindowFullscreen()
        DrawText(TextFormat("IsWindowMaximized: %i", IsWindowMaximized()), 10, 170, 5, WHITE);

        // Draw render size
        DrawText(TextFormat("Render: %i, %i", GetRenderWidth(), GetRenderHeight()), 10, 180, 5, WHITE);
    }
    // Draw FPS
    DrawFPS(virtual_screen_width - 100, 10);
}

void GameManager::SpawnAsteroid(float delta_time)
{
    asteriod_cooldown += delta_time;
    if (asteriod_cooldown > asteriod_cooldown_time)
    {
        // Get camera view boundaries
        float cameraLeftEdge = camera.target.x - (virtual_screen_width / (2.0f * camera.zoom));
        float cameraRightEdge = camera.target.x + (virtual_screen_width / (2.0f * camera.zoom));
        float cameraTopEdge = camera.target.y - (virtual_screen_height / (2.0f * camera.zoom));
        float cameraBottomEdge = camera.target.y + (virtual_screen_height / (2.0f * camera.zoom));

        // Randomly choose which edge to spawn from (0: top, 1: right, 2: bottom, 3: left)
        int spawnEdge = GetRandomValue(0, 3);
        Vector2 spawnPos = {0, 0};

        switch (spawnEdge)
        {
        case 0: // Top
            spawnPos.x = GetRandomValue(cameraLeftEdge - 50, cameraRightEdge + 50);
            spawnPos.y = cameraTopEdge - 100;
            break;
        case 1: // Right
            spawnPos.x = cameraRightEdge + 100;
            spawnPos.y = GetRandomValue(cameraTopEdge - 50, cameraBottomEdge + 50);
            break;
        case 2: // Bottom
            spawnPos.x = GetRandomValue(cameraLeftEdge - 50, cameraRightEdge + 50);
            spawnPos.y = cameraBottomEdge + 100;
            break;
        case 3: // Left
            spawnPos.x = cameraLeftEdge - 100;
            spawnPos.y = GetRandomValue(cameraTopEdge - 50, cameraBottomEdge + 50);
            break;
        }
        // Testing
        if(GetRandomValue(0,1) == 1){
            spawnPos = player->position;
            spawnPos.x += 50;
        }
        // calculate direction to camera center
        Vector2 direction = Vector2Normalize(Vector2Subtract(camera.target, spawnPos));

        asteriod_cooldown = 0.0f;
        std::shared_ptr<AstronomicalObject> asteroid = AstronomicalObject::Create(ObjectType::ASTEROID_TYPE, 100.0f, 10.0f, 1, spawnPos, {.2f, .2f}, 50.0f, 100.0f);
        
        PhysicsSystem::GetInstance().ApplyForce(asteroid->physics_id, 10, direction);
        // random torque
        PhysicsSystem::GetInstance().ApplyTorque(asteroid->physics_id, GetRandomValue(-100, 100));
        physic_objects.push_back(asteroid);
    }
}

bool GameManager::isGameOver()
{
    return isGameOver_;
}

bool GameManager::LoadMap()
{
    return true;
}

int GameManager::MenuButtom(Rectangle buttom, const char *buttom_text)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), buttom))
    {
        return true;
    }
    DrawRectangleRec(buttom, GRAY);

    DrawText(buttom_text, buttom.x + 20, buttom.y + buttom.height / 2 - 10, 20, WHITE);
    return 0;
}