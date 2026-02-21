#ifndef SDL3WGPU_H_INCLUDED
#define SDL3WGPU_H_INCLUDED






#include "SDL3/SDL_video.h"
#include "webgpu/webgpu.h"
#include <intollib.h>




typedef struct _WindowData {
    SDL_Window* window;
    WGPUSurface surface;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUPipelineLayout pipelineLayout;
} WindowData;



IStatus SetupRendererForWindow(WindowData* data);
IStatus CleanupRendererForWindow(WindowData* data);












#endif