#include "SDL3/SDL_render.h"
#include "intollib.h"
#include "sdl3data.h"






ILIB_API void DrawRectangleLine(Window* window, IRectangle rect, Color color){
    WindowData* data = (WindowData*)window->winData;
    Colorf sdlColor = {(float)color.r, 
                        (float)color.g,
                        (float)color.b,
                        (float)color.a};
    SDL_SetRenderDrawColorFloat(data->renderer, 
        color.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_FRect frect = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
    SDL_RenderRect(data->renderer, &frect);
}

ILIB_API void DrawRectangle(Window* window, IRectangle rect, Color color){
    WindowData* data = (WindowData*)window->winData;
    Colorf sdlColor = {(float)color.r, 
                        (float)color.g,
                        (float)color.b,
                        (float)color.a};
    SDL_SetRenderDrawColorFloat(data->renderer, 
        color.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_FRect frect = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
    SDL_RenderFillRect(data->renderer, &frect);
}