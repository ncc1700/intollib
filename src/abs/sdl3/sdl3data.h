#ifndef SDL3DATA_H_INCLUDED
#define SDL3DATA_H_INCLUDED








#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "intollib.h"
typedef struct _WindowData {
    Arena* arena;
    SDL_Window* window;
    SDL_Renderer* renderer;
} WindowData;





IStatus SetupRendererForWindow(WindowData* data);
IStatus CleanupRendererForWindow(WindowData* data);
u8 KeyCodeToChar(u8 keyCode);
u8 SDLMouseToIntollibMouse(u8 sdlMouse);

















#endif