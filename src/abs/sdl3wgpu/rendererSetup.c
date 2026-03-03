#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "extern/sdl3webgpu.h"
#include "intollib.h"
#include "sdl3wgpu.h"
#include "webgpu/webgpu.h"
#include <corecrt_search.h>
#include <math.h>
#include "rendererTools.h"

struct _WGPUAdapterResponse {
    WGPUAdapter adapter;
    bool reqEnded;
};

struct _WGPUDeviceResponse {
    WGPUDevice device;
    bool reqEnded;
};

static WGPUInstance instance = {0};


 

ILIB_API IStatus SetupRenderer(){
    instance = wgpuCreateInstance(NULL);
    if(instance == NULL){
        SysDebug(FAIL, QSTR("couldn't set up WGPU"));
        return ISTATUS_WGPU_FAIL;
    }
    SysDebug(PASS, QSTR("WGPU has been started"));
    return ISTATUS_SUCCESS;
}

static void HandleAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, 
                WGPUStringView message, void* userdata1, void* userdata2)
{
    struct _WGPUAdapterResponse* response = (struct _WGPUAdapterResponse*)userdata1;
    if(status == WGPURequestAdapterStatus_Success){
        response->adapter = adapter;
    } else {
        SysDebug(FAIL, QSTR("adapter failed with status of %d"), status);
        SysDebug(UNKNOWN, WGPUStringViewToIntollibString(message));
        response->adapter = NULL;
    }
    response->reqEnded = true;
}

static void HandleDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, 
                WGPUStringView message, void* userdata1, void* userdata2)
{
    struct _WGPUDeviceResponse* response = (struct _WGPUDeviceResponse*)userdata1;
    if(status == WGPURequestDeviceStatus_Success){
        response->device = device;
    } else {
        SysDebug(FAIL, QSTR("device failed with status of %d"), status);
        SysDebug(UNKNOWN, WGPUStringViewToIntollibString(message));
        response->device = NULL;
    }
    response->reqEnded = true;
}

static WGPUAdapter CreateAdapter(WGPUSurface surface, bool sync){
    struct _WGPUAdapterResponse response = {0};
    WGPURequestAdapterCallbackInfo callbackInfo = {0};
    callbackInfo.callback = HandleAdapterCallback;
    callbackInfo.userdata1 = (void*)&response;
    WGPURequestAdapterOptions options = {0};
    options.compatibleSurface = surface;
    wgpuInstanceRequestAdapter(instance, &options, callbackInfo);
    if(sync){
        while(!response.reqEnded){}; // TODO: do some sleeping
    }
    return response.adapter;
}

static WGPUDevice CreateDevice(WGPUAdapter adapter, bool sync){
    struct _WGPUDeviceResponse response = {0};
    WGPURequestDeviceCallbackInfo callbackInfo = {0};
    callbackInfo.callback = HandleDeviceCallback;
    callbackInfo.userdata1 = (void*)&response;
    WGPUDeviceDescriptor desc = {0};
    // desc.deviceLostCallbackInfo
    // TODO: setup callbacks
    wgpuAdapterRequestDevice(adapter, NULL, callbackInfo);
    if(sync){
        while(!response.reqEnded){} // TODO: do some sleeping
    }
    return response.device;
}

static WGPUCommandEncoder CreateCommandEncoder(WGPUDevice device, String label){
    WGPUCommandEncoderDescriptor desc = {0};
    desc.label = IntollibStringToWGPUStringView(label);
    desc.nextInChain = NULL;
    return wgpuDeviceCreateCommandEncoder(device, &desc);
}

IStatus SetupRendererForWindow(WindowData* data){
    if(instance == NULL){
        return ISTATUS_NOT_INITIALIZED;
    }
    SysDebug(INFO, QSTR("creating renderer for window...."));
    data->surface = SDL_GetWGPUSurface(instance, data->window);
    if(data->surface == NULL){
        SysDebug(FAIL, QSTR("couldn't get wgpu surface"));
        return ISTATUS_WGPU_FAIL;
    }
    data->adapter = CreateAdapter(data->surface, true);
    if(data->adapter == NULL){
        SysDebug(FAIL, QSTR("couldn't get wgpu adapter"));
        return ISTATUS_WGPU_FAIL;
    }
    PrintAdapterData(data->adapter);
    data->device = CreateDevice(data->adapter, true);
    if(data->device == NULL){
        SysDebug(FAIL, QSTR("couldn't get wgpu device"));
        return ISTATUS_WGPU_FAIL;
    }
    data->queue = wgpuDeviceGetQueue(data->device);
    if(data->queue == NULL){
        SysDebug(FAIL, QSTR("couldn't get queue from device"));
        return ISTATUS_WGPU_FAIL;
    }
    // TODO: setup callbacks for queue

    data->commandEncoder = CreateCommandEncoder(data->device, QSTR("placeholder commandencoder name"));
    
    SysDebug(PASS, QSTR("render for window has been created...."));
    return ISTATUS_SUCCESS;
}

ILIB_API void BeginDrawing(Window* window, Colorf backgroundColor){
    WindowData* data = (WindowData*)window->winData;
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
    }
}   


ILIB_API void DrawHelloTriangle(Window* window){
    
}

ILIB_API void DrawRectangle(Window* window, Vec2d position){
    
}



ILIB_API void EndDrawing(Window* window){
    
  
    return;
}

IStatus CleanupRendererForWindow(WindowData* data){
    wgpuQueueRelease(data->queue);
    wgpuDeviceRelease(data->device);
    wgpuAdapterRelease(data->adapter);
    wgpuSurfaceRelease(data->surface);
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus CleanupRenderer(){
    wgpuInstanceRelease(instance);
    return ISTATUS_SUCCESS;
}