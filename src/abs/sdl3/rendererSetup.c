#include "SDL3/SDL_render.h"
#include "intollib.h"
#include "sdl3data.h"
#include <math.h>









ILIB_API IStatus SetupRenderer(){
    return ISTATUS_SUCCESS;
}

IStatus SetupRendererForWindow(WindowData* data){
    data->renderer = SDL_CreateRenderer(data->window, NULL);
    if(data->renderer == NULL){
        SysDebug(FAIL, QSTR("couldn't create renderer"));
        return ISTATUS_SDL_FAIL;
    }
    return ISTATUS_SUCCESS;
}


IStatus CleanupRendererForWindow(WindowData* data){
    SDL_DestroyRenderer(data->renderer);
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus CleanupRenderer(){
    return ISTATUS_SUCCESS;
}
