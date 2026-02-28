#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_video.h"
#include "extern/sdl3webgpu.h"
#include "sdl3wgpu.h"
#include "webgpu/webgpu.h"
#include <SDL3/SDL.h>
#include <intollib.h>


ILIB_API IStatus SetupWindow(){
    bool result = SDL_Init(SDL_INIT_VIDEO);
    if(!result){
        SysDebug(FAIL, QSTR("couldn't set up SDL3"));
        return ISTATUS_SDL_FAIL;
    }
    SysDebug(PASS, QSTR("SDL3 has been started"));
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus InitWindow(Window* window, String title, u32 width, u32 height){
    Arena arena = {0};
    IStatus status = CreateArena(&arena, 
            ((title.length + 10) * sizeof(char)));
    if(!IS_SUCCESS(status)){
        SysDebug(FAIL, QSTR("couldn't create local arena for %s()"), __FUNCTION__);
        return status;
    }
    status = CreateArena(&window->arena, 
            MB(10));
    if(!IS_SUCCESS(status)){
        SysDebug(FAIL, QSTR("couldn't create window level arena"));
        status = ISTATUS_OUT_OF_MEMORY;
        goto EXIT;
    }
    WindowData* data = PushMemoryFromArena(&window->arena, 
            sizeof(WindowData));
    if(data == NULL){
        SysDebug(FAIL, QSTR("couldn't get memory for windowdata"));
        status = ISTATUS_OUT_OF_MEMORY;
        goto EXIT;
    }
    char* cstr = QStringToCStringArena(&arena, title);
    if(cstr == NULL){
        SysDebug(FAIL, QSTR("couldn't get memory for cstr"));
        status = ISTATUS_OUT_OF_MEMORY;
        goto EXIT;
    }
    SDL_Init(SDL_INIT_VIDEO);
    data->window = (void*)SDL_CreateWindow((const char*)cstr, 
                    width, height, 0);
    if(data->window == NULL){
        SysDebug(FAIL, QSTR("couldn't create SDL window"));
        status = ISTATUS_SDL_FAIL;
        goto EXIT;
    }
    data->arena = &window->arena;
    SetupRendererForWindow(data);
    window->height = height;
    window->width = width;
    window->isRunning = TRUE;
    window->winData = (void*)data;
EXIT:
    DestroyArena(&arena);
    return status;
}

ILIB_API bool WindowIsRunning(Window* window){
    return window->isRunning;
}



ILIB_API IStatus DeleteWindow(Window* window){
    WindowData* data = (WindowData*)window->winData;
    CleanupRendererForWindow(data);
    SDL_DestroyWindow(data->window);
    DestroyArena(&window->arena);
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus CleanupWindow(){
    SDL_Quit();
    
    return ISTATUS_SUCCESS;
}