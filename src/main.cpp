#include "raylib.h"
#include "game_manager.h"
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

void CustomLog(int msgType, const char *text, va_list args)
{
    switch (msgType)
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
const int screenWidth = 640;
const int screenHeight = 360;
const float targetFrameTime = 0.02f; // Target 50 FPS
float accumulator = 0.0f;
GameManager *gameManager;
bool is_game_fullscreen = false;

void UpdateDrawFrame(void)
{
    if (!is_game_fullscreen && IsWindowFullscreen())
    {
#ifdef __EMSCRIPTEN__
        SetWindowSize(GetMonitorWidth(0),
                    GetMonitorHeight(0));
#else
        SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                    GetMonitorHeight(GetCurrentMonitor()));
#endif
        is_game_fullscreen = true;
    }else if(is_game_fullscreen){
        is_game_fullscreen = false;
    }
    // Measure time elapsed since last frame
    float dt = GetFrameTime();
    // Update game manager
    if(IsWindowFocused()){
        gameManager->Update(dt);
        // Fix Update for physics
        accumulator += dt;
        while (accumulator >= targetFrameTime)
        {
            gameManager->FixUpdate(targetFrameTime);
            accumulator -= targetFrameTime;
        }
    }
    BeginDrawing();
    // All drawing happens here
    if(IsWindowFocused()){
        gameManager->Render();
    }

    EndDrawing();

}

int main()
{
    SetTraceLogCallback(CustomLog);
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenWidth, screenHeight, "Depths of Iara");
    
// check if windows
#if defined(_WIN32)
    ToggleFullscreen();
#endif

    if (IsWindowFullscreen())
    {
        SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                      GetMonitorHeight(GetCurrentMonitor()));
    }
    SetWindowState(FLAG_WINDOW_RESIZABLE);


    // Create a GameManager instance
    gameManager = new GameManager();
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    //SetTargetFPS(0);
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    
    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    delete gameManager;
    CloseWindow();

    return 0;
}