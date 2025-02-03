#ifndef PLANET_H
#define PLANET_H

#include "raylib.h"
#include "dynamic_body.h"
#include <vector>

class Planet : public DynamicBody
{
public:
    // Attributes
    float life = 100.0f;
    float max_life = 100.0f;
    float temperature = 0.0f;
    float damage = 0.0f;
    bool is_alive = true;
    Texture2D planet_texture;

    static std::shared_ptr<Planet> Create(Vector2 in_position){
        std::shared_ptr<Planet> obj = std::make_shared<Planet>(in_position);
        obj->object_type = ObjectType::ASTEROID_TYPE;
        obj->physics_id = PhysicsObject::CreatePhysicsId(obj);
        TraceLog(LOG_INFO, "Object of type Planet created");
        return obj;
    }
    
    Planet(Vector2 in_position){
        position = in_position;
        // Planet 1 Data (Simple Ringed)
        std::vector<std::vector<Color>> planet1 = {
            {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, DARKGRAY, DARKGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, DARKGRAY, LIGHTGRAY, LIGHTGRAY, DARKGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK},
            {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK}
        };
        Image planet1_image = GenImageColor(16, 16, BLANK);
        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                ImageDrawPixel(&planet1_image, x, y, planet1[y][x]);
            }
        }
        ImageResizeNN(&planet1_image, 128, 128);
        if (IsWindowReady()){
            // Generate a Texture2D from the color data
            planet_texture = LoadTextureFromImage(planet1_image);
        }else{
            planet_texture = {0};
        }

        // Unload the image from memory
        UnloadImage(planet1_image);
        height = 128;
        width = 128;
    }
    ~Planet()
    {
        if (IsWindowReady() == false)
            return;
        if (planet_texture.id > 0)
        {
            UnloadTexture(planet_texture);
        }
        TraceLog(LOG_INFO, "Planet destroyed");
    }
    void Render() override
    {
        if (!IsWindowReady())
            return;
        if (!is_alive)
            return;
        // Draw the planet texture
        DrawTexture(planet_texture, position.x, position.y, WHITE);
    }
};

#endif // PLANET_H