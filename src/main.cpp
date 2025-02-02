#include "raylib.h"
#include "game_manager.h"
#include "global.h"
#include <iostream>
#include <string>
#include <cstring>


#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame(void);

void CustomLog(int msg_type, const char *text, va_list args)
{
    switch (msg_type)
    {
    case LOG_INFO:
        // check if text starts with TIMER:
        if (strncmp(text, "TIMER:", 6) == 0)
        {
            // // Extract the time value from the text
            // int time;
            // sscanf(text, "TIMER: %d", &time);
            // std::cout << "[TIMER]: " << time << " ms" << std::endl;
        }
        else
        {
            std::cout << "[INFO] : " << text << std::endl;
        }
        break;
    case LOG_ERROR:
        std::cout << "[ERROR]: " << text << std::endl;
        break;
    case LOG_WARNING:
        std::cout << "[WARN] : " << text << std::endl;
        break;
    case LOG_DEBUG:
        std::cout << "[DEBUG]: " << text << std::endl;
        break;
    default:
        break;
    }
}
// Initialization
int screen_width = 640;
int screen_height = 360;
int virtual_screen_width = 640;
int virtual_screen_height = 360;
const float target_frame_time = 0.02f; // Target 50 FPS

float scale = (float)screen_height / (float)virtual_screen_height;
float accumulator = 0.0f;
GameManager *game_manager;
bool is_game_fullscreen = false;
float dt = -1;
RenderTexture2D target;
void UpdateDrawFrame(void)
{
    if (!is_game_fullscreen && IsWindowFullscreen())
    {
#ifdef __EMSCRIPTEN__
        SetWindowSize(GetMonitorWidth(0),
                    GetMonitorHeight(0));
        is_game_fullscreen = true;
#else
        SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                    GetMonitorHeight(GetCurrentMonitor()));
        is_game_fullscreen = true;
#endif
        is_game_fullscreen = true;
    }else if(is_game_fullscreen){
        is_game_fullscreen = false;
    }
    if(IsWindowResized()){
        screen_height = GetScreenHeight();
        screen_width = GetScreenWidth();
        scale = (float)screen_height / (float)virtual_screen_height;
    }
    // Measure time elapsed since last frame
    dt = GetFrameTime();
    // Update game manager
    if(IsWindowFocused()){
        game_manager->Update(dt);
        // Fix Update for physics
        accumulator += dt;
        while (accumulator >= target_frame_time)
        {
            game_manager->FixUpdate(accumulator);
            accumulator -= accumulator;
        }
        TraceLog(LOG_DEBUG, TextFormat("Update Time: %f accumulator: %f", dt, accumulator));
    }
    BeginTextureMode(target);
    // All drawing happens here
    if(IsWindowFocused()){
        game_manager->Render();
    }

    EndTextureMode();

    BeginDrawing();
        Rectangle source_rec = {0,0, (float)target.texture.width, (float)-target.texture.height};
        Rectangle dest_rec = {0,0, (float)virtual_screen_width * scale, (float)virtual_screen_height * scale};

        DrawTexturePro(target.texture, source_rec, dest_rec, { 0, 0 }, 0, WHITE);
    EndDrawing();
}

#ifndef TESTING
int main()
{
    SetTraceLogCallback(CustomLog);
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screen_width, screen_height, "Depths of Iara");
    target = LoadRenderTexture(virtual_screen_width, virtual_screen_height);
// check if windows
#if defined(_WIN32)
    ToggleFullscreen();
#endif

    if (IsWindowFullscreen())
    {

#ifdef __EMSCRIPTEN__
        SetWindowSize(GetMonitorWidth(0),
                    GetMonitorHeight(0));
        is_game_fullscreen = true;
#else
        SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                    GetMonitorHeight(GetCurrentMonitor()));
        is_game_fullscreen = true;
#endif
    }
    // Create a Game_manager instance
    game_manager = new GameManager();
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    //SetTargetFPS(0);
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    
    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    delete game_manager;
    CloseWindow();

    return 0;
}
#endif