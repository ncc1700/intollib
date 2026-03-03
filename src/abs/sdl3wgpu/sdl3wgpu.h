#ifndef SDL3WGPU_H_INCLUDED
#define SDL3WGPU_H_INCLUDED






#include "SDL3/SDL_video.h"
#include "webgpu/webgpu.h"
#include <intollib.h>




typedef struct _WindowData {
    Arena* arena;
    WGPUBindGroup bindGroup;
    WGPUBindGroupLayout bGroupLayout;
    WGPUBuffer fragBuf;
    WGPUBuffer vertexUniBuf;
    WGPUCommandEncoder commandEncoder;
    WGPURenderPassEncoder renderPassEncoder;
    WGPUSurfaceTexture surfaceText;
    WGPUTextureView frame;
    WGPUSurfaceCapabilities surfaceCapablities;
    WGPUSurfaceConfiguration surfaceConfig;
    SDL_Window* window;
    WGPUCommandBuffer cBuffer;
    WGPURenderPipeline renderPipeline;
    WGPUSurface surface;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUPipelineLayout pipelineLayout;
    WGPUShaderModule shaderMod;
    bool doStuffInEndDrawing;
} WindowData;



IStatus SetupRendererForWindow(WindowData* data);
IStatus CleanupRendererForWindow(WindowData* data);












#endif