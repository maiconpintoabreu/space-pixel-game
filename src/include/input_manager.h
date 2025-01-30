#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <memory>

class InputManager
{
public:
    Vector2 touch_start{};

private:
    bool is_touch_enabled = false;
    std::weak_ptr<Player> player;

    bool is_turning_left = false;
    bool is_turning_right = false;
    bool is_accelerating = false;
    bool is_shooting = false;
    Rectangle touch_left_area = Rectangle();
    Rectangle touch_right_area = Rectangle();
    Rectangle touch_up_area = Rectangle();
    Rectangle touch_shoot_area = Rectangle();
    Image turn_image;
    Image shoot_image;

    Texture2D turn_left_texture;
    Texture2D turn_right_texture;
    Texture2D accelerate_texture;
    Texture2D shoot_texture;
    float size = 50.0f;
    bool is_initialized = false;

public:
    InputManager() = default;
    ~InputManager()
    {

        if (IsWindowReady() == false)
            return;
        if (turn_left_texture.id > 0)
        {
            UnloadTexture(turn_left_texture);
        }
        if (turn_right_texture.id > 0)
        {
            UnloadTexture(turn_right_texture);
        }
        if (accelerate_texture.id > 0)
        {
            UnloadTexture(accelerate_texture);
        }
        if (shoot_texture.id > 0)
        {
            UnloadTexture(shoot_texture);
        }
        TraceLog(LOG_INFO, "InputManager destroyed");
    }

    void SetPlayer(std::shared_ptr<Player> in_player)
    {
        // set player from shared_ptr to weak_ptr
        player = in_player;
        LoadGUI();
        is_initialized = true;
    }
    void LoadGUI()
    {
        size = 100.0f * (GetScreenWidth() + GetScreenHeight()) / 1000;
        touch_left_area = {10, static_cast<float>(GetScreenHeight()) - (size + 20), size, size};
        touch_right_area = {(size + 20), static_cast<float>(GetScreenHeight()) - (size + 20), size, size};
        touch_up_area = {static_cast<float>(GetScreenWidth()) - (size + 10), static_cast<float>(GetScreenHeight()) - (size + 20), size, size};
        touch_shoot_area = {static_cast<float>(GetScreenWidth()) - (size * 2 + 20), static_cast<float>(GetScreenHeight()) - (size + 20), size, size};

        turn_image = GenImageColor(size, size, BLANK);
        ImageDrawTextEx(&turn_image, GetFontDefault(), "<", {16 * size / 50.0f, 3 * size / 50.0f}, size, 0.0f, WHITE);
        turn_left_texture = LoadTextureFromImage(turn_image);

        ImageRotate(&turn_image, 180.0f);
        turn_right_texture = LoadTextureFromImage(turn_image);

        ImageRotate(&turn_image, -90.0f);
        accelerate_texture = LoadTextureFromImage(turn_image);

        shoot_image = GenImageColor(size, size, BLANK);
        ImageDrawTextEx(&shoot_image, GetFontDefault(), "+", {13 * size / 50.0f, 2 * size / 50.0f}, size, 1.0f, WHITE);
        shoot_texture = LoadTextureFromImage(shoot_image);

        UnloadImage(shoot_image);
        UnloadImage(turn_image);
    }
    void Update(float delta_time)
    {
        if (!is_initialized)
            return;
        if (!is_touch_enabled && GetTouchPointCount() > 0)
        {
            is_touch_enabled = true;
        }

        if (IsWindowResized())
        {
            LoadGUI();
        }

        // Keyboard controls
        HandleKeyboardInput();
        // Touch controls
        HandleTouchInput();
    }
    void FixUpdate()
    {
        if (!is_initialized) return;
        if (!is_touch_enabled) return;
        if (IsButtonPressed(touch_left_area))
        {
            is_turning_left = true;
        }
        else
        {
            is_turning_left = false;
        }
        if (IsButtonPressed(touch_right_area))
        {
            is_turning_right = true;
        }
        else
        {
            is_turning_right = false;
        }
        if (IsButtonPressed(touch_up_area))
        {
            is_accelerating = true;
        }
        else
        {
            is_accelerating = false;
        }
        if (IsButtonPressed(touch_shoot_area))
        {
            is_shooting = true;
        }
        else
        {
            is_shooting = false;
        }
    }

private:
    void HandleKeyboardInput()
    {
        if (is_touch_enabled)
        {
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) ||
                IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN) ||
                IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ||
                IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) ||
                IsKeyDown(KEY_SPACE))
            {
                is_touch_enabled = false;
            }
            else
            {
                return;
            }
        }
        if (auto shared_player = player.lock())
        {
            // Forward movement
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            {
                shared_player->Accelerate();
            }

            // Backward movement
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            {
                shared_player->Decelerate();
            }

            // Rotation left
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            {
                shared_player->TurnLeft();
            }

            // Rotation right
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
            {
                shared_player->TurnRight();
            }

            // Shot
            if (IsKeyDown(KEY_SPACE))
            {
                shared_player->Shoot();
            }
        }
    }

    void HandleTouchInput()
    {
        if (auto shared_player = player.lock())
        {
            if (is_turning_left)
            {
                shared_player->TurnLeft();
            }
            if (is_turning_right)
            {
                shared_player->TurnRight();
            }
            if (is_accelerating)
            {
                shared_player->Accelerate();
            }
            if (is_shooting)
            {
                shared_player->Shoot();
            }
        }
    }
    // Check if mouse or touching is pressing a buttom
    bool IsButtonPressed(Rectangle button)
    {
        if (is_touch_enabled)
        {
            for (int i = 0; i < GetTouchPointCount(); i++)
            {
                if (CheckCollisionPointRec(GetTouchPosition(i), button))
                {
                    return true;
                }
            }
        }
        else
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button))
            {
                return true;
            }
        }
        return false;
    }

public:
    void Render()
    {
        if (!is_initialized)
            return;
        if (is_touch_enabled)
        {
            // Set style for touch buttons
            DrawRectangleRec(touch_left_area, {0, 0, 0, 10});
            DrawTexture(turn_left_texture, touch_left_area.x, touch_left_area.y, WHITE);
            DrawRectangleRec(touch_right_area, {0, 0, 0, 10});;
            DrawTexture(turn_right_texture, touch_right_area.x, touch_right_area.y, WHITE);
            DrawRectangleRec(touch_up_area, {0, 0, 0, 10});
            DrawTexture(accelerate_texture, touch_up_area.x, touch_up_area.y, WHITE);
            DrawRectangleRec(touch_shoot_area, {0, 0, 0, 10});;
            DrawTexture(shoot_texture, touch_shoot_area.x, touch_shoot_area.y, WHITE);
        }
        if (auto shared_player = player.lock())
        {
            DrawRectangle(20, 20, 50, 20, GRAY);
            DrawRectangle(21, 21, 48 * shared_player->GetPercentHealth(), 18, RED);
            DrawRectangle(20, 20 + 22, 50, 20, GRAY);
            DrawRectangle(21, 21 + 22, 48 * shared_player->GetPercentEnergy(), 18, YELLOW);
        }
    }
};

#endif // INPUT_MANAGER_H