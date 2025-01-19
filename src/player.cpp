#include "player.h"
#include "raymath.h"
#include "enums.h"

Player::Player()
{
    // Initialize player physics
    position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    // Generate a image for the spaceship
    Image spaceship_image = GenImageColor(16, 16, BLANK);

    // Draw spaceship Base
    ImageDrawRectangleV(&spaceship_image, {5, 6}, {6, 8}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, {4, 8}, {1, 4}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, {11, 8}, {1, 4}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, {7, 7}, {2, 5}, GRAY);
    ImageDrawPixelV(&spaceship_image, {5, 10}, GRAY);
    ImageDrawPixelV(&spaceship_image, {10, 10}, GRAY);
    ImageDrawPixelV(&spaceship_image, {7, 5}, LIGHTGRAY);
    ImageDrawPixelV(&spaceship_image, {8, 5}, LIGHTGRAY);
    // Draw spaceship Gun
    // ImageDrawPixelV(&spaceship_image, { 5, 5 }, LIGHTGRAY);
    // ImageDrawPixelV(&spaceship_image, { 10, 5 }, LIGHTGRAY);
    // Draw Propeller
    ImageDrawRectangleV(&spaceship_image, {4, 13}, {2, 2}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, {10, 13}, {2, 2}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, {4, 15}, {2, 1}, DARKGRAY);
    ImageDrawRectangleV(&spaceship_image, {10, 15}, {2, 1}, DARKGRAY);

    Image propeller_image_left = GenImageColor(16, 19, BLANK);
    ImageDrawPixelV(&propeller_image_left, {4, 16}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_left, {5, 17}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_left, {4, 18}, LIGHTGRAY);
    Image propeller_image_right = GenImageColor(16, 19, BLANK);
    ImageDrawPixelV(&propeller_image_right, {10, 16}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_right, {11, 17}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_right, {10, 18}, LIGHTGRAY);

    if (IsWindowReady() == true)
    {
        // Load spaceship texture
        spaceship = LoadTextureFromImage(spaceship_image);
    }
    else
    {
        spaceship = {0};
    }

    UnloadImage(spaceship_image);
    origin = {spaceship.width / 2.0f, (spaceship.height / 2.0f) + 2};
    gun_socket_left = {7, 5};
    gun_socket_right = {8, 5};
    rotation_speed = 2.0f;
    rotation_speed_limit = 50.0f;
    speed = 40.0f;
    deceleration_multiplier = 1.0f;
    object_type = ObjectType::PLAYER_TYPE;
    height = 10.0f;
    width = spaceship.width;
    center = origin;
    is_on_screen = true;
    gun_cooldown_time = 0.1f;
    // float thruster_offset_x = spaceship.width * 0.2f;
    // float thruster_offset_y = spaceship.height * 0.4f;

    // Vector2 left_thruster_pos = { position.x - thruster_offset_x, position.y + thruster_offset_y };
    // Vector2 right_thruster_pos = { position.x + thruster_offset_x, position.y + thruster_offset_y };
}
// Spaceship physics logic
void Player::FixUpdate(float delta_time)
{
    // Call the base class version
    DynamicBody::FixUpdate(delta_time);
}
// Spaceship movement logic
void Player::Update(float delta_time)
{
    // Update gun cooldown
    if (!is_gun_ready)
    {
        gun_cooldown -= delta_time;
        if (gun_cooldown < 0.0f)
        {
            gun_cooldown = 0.0f;
            is_gun_ready = true;
        }
    }
    direction = {
        sinf(rotation * DEG2RAD),
        -cosf(rotation * DEG2RAD)};
    gun_position = Vector2Add(position, {direction.x * (spaceship.width - 10.0f), direction.y * (spaceship.height - 10.0f)});
    for (size_t i = bullets.size()-1; i >= 0; i--)
    {
        if (auto shared_bullet = bullets.at(i).lock())
        {
            if (shared_bullet->is_alive)
            {
                shared_bullet->Update(delta_time);
                while (!shared_bullet->eventQueue.empty()) {
                    BulletEvent event = shared_bullet->eventQueue.front();
                    shared_bullet->eventQueue.pop();

                    switch (event.type) {
                        case BulletEventType::COLLISION:
                        shared_bullet->Destroy();
                        score += 10;
                        break;
                        default:
                        break;
                    }
                }
            }
            else
            {
                bullets.erase(bullets.begin() + i);
            }
        }else
        {
            bullets.erase(bullets.begin() + i);
        }
    }
    // // Update Thruster Position
    // float thruster_offset_x = spaceship.width * 0.2f;
    // float thruster_offset_y = spaceship.height * 0.4f;

    // Left thruster
    // Vector2 left_thruster_offset = {
    //     thruster_offset_x * direction.x - thruster_offset_y * direction.y,
    //     thruster_offset_x * direction.y + thruster_offset_y * direction.x
    // };
    
    // Vector2 left_thruster_pos = {
    //     position.x + left_thruster_offset.x,
    //     position.y + left_thruster_offset.y
    // };

    // Right thruster
    // Vector2 right_thruster_offset = {
    //     thruster_offset_x * direction.x - thruster_offset_y * direction.y,
    //     thruster_offset_x * direction.y + thruster_offset_y * direction.x
    // };
    
    // Vector2 right_thruster_pos = {
    //     position.x + right_thruster_offset.x,
    //     position.y + right_thruster_offset.y
    // };

    // thruster_left.SetEmitterPosition(left_thruster_pos);
    // thruster_right.SetEmitterPosition(right_thruster_pos);

    // // Update the particle systems
    // thruster_left.Update(delta_time);
    // thruster_right.Update(delta_time);;
}

void Player::Render()
{ 
    // Call the base class version
    DynamicBody::Render();
    // Check if window is ready
    if (IsWindowReady() == false)
        return;
    if (!is_on_screen)
        return;
    DrawTexturePro(
        spaceship,
        Rectangle({0, 0, static_cast<float>(spaceship.width), static_cast<float>(spaceship.height)}),                   // Source rectangle (full texture)
        Rectangle({position.x, position.y, static_cast<float>(spaceship.width), static_cast<float>(spaceship.height)}), // Destination rectangle (position and size)
        origin,                                                                                                         // Origin of rotation (center of the texture)
        rotation,                                                                                                       // Rotation in degrees
        WHITE                                                                                                           // Tint color
    );

    if (is_accelerating || is_rotating_right)
    {
        // Draw rotated propeller texture Left
        // thruster_left.Draw();
    }
    if (is_accelerating || is_rotating_left)
    {
        // Draw rotated propeller texture Right
        // thruster_right.Draw();
    }
    // Render bullets that are enabled
    for (std::weak_ptr<Bullet> bullet : bullets)
    {
        if (auto shared_bullet = bullet.lock())
        {
            if (shared_bullet->is_enabled)
            {
                shared_bullet->Render();
            }
        }
    }
    DrawCircleV(gun_position, 1.0f, RED);
    DrawLineV(gun_position, gun_position + Vector2Scale(direction, gun_range), {0, 200, 0, 10});
}

void Player::TurnLeft()
{
    ApplyTorque(-rotation_speed);
}

void Player::TurnRight()
{
    ApplyTorque(rotation_speed);
}

void Player::Accelerate()
{
    ApplyForce(speed);
}

void Player::Decelerate()
{
    ApplyForce(-speed);
}

void Player::TakeDamage(float damage, Vector2 point)
{
    health -= damage;
    ApplyForceDirected(damage / max_health * 100, Vector2Normalize(position - point));
    if (health <= 0.0f)
    {
        health = 0.0f;
        is_alive = false;
    }
}

void Player::Shoot()
{
    // Shoot a bullet
    if (is_gun_ready)
    {
        is_shooting = true;
        gun_cooldown = gun_cooldown_time; // Reset cooldown
        is_gun_ready = false;
        std::shared_ptr<Bullet> bullet = Bullet::Create();
        bullet->owner = shared_from_this();
        bullet->position = gun_position;
        bullet->rotation = rotation;
        // Vector2 direction = {sin(rotation*DEG2RAD), -cos(rotation*DEG2RAD)};
        bullet->SetEnabled(true);
        PhysicsSystem::GetInstance().ApplyForce(bullet->physics_id, 500, direction);
        bullets.push_back(bullet);
    }

    // TODO: Add raycast collision detection here when needed
}
