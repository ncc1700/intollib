#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <intollib.h>

ILIB_API IStatus SetupWindow(){
    SDL_Init(SDL_INIT_VIDEO);;
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus InitWindow(Window* window, String title, u32 width, u32 height){
    Arena arena = {0};
    IStatus status = CreateArena(&arena, 
            ((title.length + 10) * sizeof(char)));
    if(!IS_SUCCESS(status)){
        return status;
    }
    char* cstr = QStringToCStringArena(&arena, title);
    if(cstr == NULL){
        status = ISTATUS_OUT_OF_MEMORY;
        goto EXIT;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* sdlWin = SDL_CreateWindow((const char*)cstr, width, height, 0);
    if(sdlWin == NULL){
        status = ISTATUS_CANT_CREATE_WINDOW;
        goto EXIT;
    }
    window->height = height;
    window->width = width;
    window->isRunning = TRUE;
    window->winData = (void*)sdlWin;
EXIT:
    DestroyArena(&arena);
    return status;
}

ILIB_API bool WindowIsRunning(Window* window){
    return window->isRunning;
}

ILIB_API void BeginDrawing(Window* window){
    //SDL_Window* sdlWin = (SDL_Window*)window->winData;
    SDL_Event event = {0};
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_EVENT_QUIT){
            window->isRunning = FALSE;
        }
    }
}

ILIB_API void EndDrawing(Window* window){
    return;
}

ILIB_API IStatus DeleteWindow(Window* window){
    SDL_Window* sdlWin = (SDL_Window*)window->winData;
    SDL_DestroyWindow(sdlWin);
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus CleanupWindow(){
    SDL_Quit();
    return ISTATUS_SUCCESS;
}