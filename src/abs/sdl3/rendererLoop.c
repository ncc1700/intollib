#include "SDL3/SDL_events.h"
#include "intollib.h"
#include "sdl3data.h"



ILIB_API void DrawHelloTriangle(Window* window){
    
}



ILIB_API void BeginDrawing(Window* window, Color backgroundColor){
    WindowData* data = (WindowData*)window->winData;
    Colorf sdlColor = {(float)backgroundColor.r, 
                        (float)backgroundColor.g,
                        (float)backgroundColor.b,
                        (float)backgroundColor.a};
    SDL_Event event = {0};
    SDL_WindowID id = SDL_GetWindowID(data->window);
    while(SDL_PollEvent(&event)){
        // being weird about this =(
        if(event.type == SDL_EVENT_QUIT){
            window->isRunning = FALSE;
        }
        if(id != event.window.windowID){
            continue;
        }
        if(event.type == SDL_EVENT_KEY_DOWN){
            window->curKey = KeyCodeToChar(event.key.key);
        } else {
            window->curKey = 0;
        }
        if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
            window->curMouse.button = SDLMouseToIntollibMouse(event.button.button);
            window->curMouse.x = event.button.x;
            window->curMouse.y = event.button.y;
        } else {
            window->curMouse.button = 0;
        }
        if(event.type == SDL_EVENT_MOUSE_MOTION){
            window->curMouse.x = event.motion.x;
            window->curMouse.y = event.motion.y;
        }
    }
    SDL_SetRenderDrawColorFloat(data->renderer, 
        sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_RenderClear(data->renderer);
    
}


ILIB_API void EndDrawing(Window* window){
    WindowData* data = (WindowData*)window->winData;
    SDL_RenderPresent(data->renderer);
}