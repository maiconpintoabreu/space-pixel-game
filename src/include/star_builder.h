#ifndef STARBUILDER_H
#define STARBUILDER_H

#include "raylib.h"
#include <vector>
#include <random>

#define MAX_STARS 5000

class StarBuilder
{
private:
    struct Star
    {
        Vector2 position;
        float speed;
        Color color;
    };

    std::vector<Star> farStars;
    std::vector<Star> nearStars;
    int screen_width;
    int screen_height;
    Vector2 camera_position;
public:
    int camera_zoom = 1;
public:
    StarBuilder() = default;

    StarBuilder(int num_stars = 500, Vector2 camera_position = {0.0f, 0.0f}, float camera_zoom = 1.0f) : camera_position(camera_position), camera_zoom(camera_zoom)
    {
        screen_width = GetScreenWidth() / camera_zoom;
        screen_height = GetScreenHeight() / camera_zoom;
        if (num_stars > MAX_STARS)
        {
            num_stars = MAX_STARS;
        }
        farStars.resize(num_stars);
        nearStars.resize(num_stars);
        initializeStars();
    }

    ~StarBuilder()
    {
        farStars.clear();
        nearStars.clear();
        TraceLog(LOG_INFO, "StarBuilder destroyed");
    }

    void Update(float delta_time)
    {
        if (IsWindowResized())
        {
            screen_width = GetScreenWidth() / camera_zoom;
            screen_height = GetScreenHeight() / camera_zoom;
            TraceLog(LOG_INFO, TextFormat("Screen size: %i x %i", screen_width, screen_height));

            TraceLog(LOG_INFO, TextFormat("Real Screen size: %i x %i", GetScreenWidth(), GetScreenHeight()));
        }
    }

    void FixUpdate(Vector2 in_camera_position)
    {
        camera_position = in_camera_position;
        for (Star &star : farStars)
        {
            star.position.x -= star.speed * GetFrameTime();
        }
        for (Star &star : nearStars)
        {
            star.position.x -= star.speed * GetFrameTime();
        }
    }

    void ResetStar(Star &star, Vector2 camera_position)
    {
        // move these out of the loop
        float right_edge = camera_position.x + screen_width/2.0f;
        float bottom_edge = camera_position.y + screen_height/2.0f;
        float left_edge = camera_position.x - screen_width/2.0f;
        float top_edge = camera_position.y - screen_height/2.0f;
        if (star.position.x < left_edge)
        {
            star.position.x = right_edge;
            star.position.y = GetRandomValue(top_edge, bottom_edge);
        }
        if (star.position.y < top_edge)
        {
            star.position.y = bottom_edge;
            star.position.x = GetRandomValue(left_edge, right_edge);
        }
        if (star.position.x > right_edge)
        {
            star.position.x = left_edge;
            star.position.y = GetRandomValue(top_edge, bottom_edge);
        }
        if (star.position.y > bottom_edge)
        {
            star.position.y = top_edge;
            star.position.x = GetRandomValue(left_edge, right_edge);
        }
    }

    void ReInitializeStars()
    {
        initializeStars();
    }

    void Render()
    {
        for (Star &star : farStars)
        {
            DrawPixelV(star.position, star.color);
            ResetStar(star, camera_position);
        }
        for (Star &star : nearStars)
        {
            DrawPixelV(star.position, star.color);
            ResetStar(star, camera_position);
        }
    }

private:
    void initializeStars()
    {
        for (auto &star : farStars)
        {
            star.position = {static_cast<float>(GetRandomValue(0, screen_width)), static_cast<float>(GetRandomValue(0, screen_height))};
            star.speed = GetRandomValue(1, 2) / 10.0f;
            star.color = {200, 200, 200, static_cast<unsigned char>(GetRandomValue(155, 255))};
        }
        for (auto &star : nearStars)
        {
            star.position = {static_cast<float>(GetRandomValue(0, screen_width)), static_cast<float>(GetRandomValue(0, screen_height))};
            star.speed = GetRandomValue(2, 6) / 10.0f;
            star.color = {255, 255, 255, static_cast<unsigned char>(GetRandomValue(155, 255))};
        }
    }

    inline bool IsPositionOnScreen(Vector2 world_position, Vector2 camera_position)
    {
        float screen_width_with_zoom = screen_width * camera_zoom;
        float screen_height_with_zoom = screen_height * camera_zoom;
        if (world_position.x >= camera_position.x && world_position.x <= camera_position.x + screen_width_with_zoom &&
            world_position.y >= camera_position.y && world_position.y <= camera_position.y + screen_height_with_zoom)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

#endif // STARBUILDER_H