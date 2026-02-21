#include "extern/sdl3webgpu.h"
#include "intollib.h"
#include "sdl3wgpu.h"
#include "webgpu/webgpu.h"


static WGPUInstance instance = {0};


static void HandleAdaptorRequest(WGPURequestAdapterStatus status, WGPUAdapter adaptor,
                                    WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)userdata2;
    WindowData* data = (WindowData*)userdata1;
    if(status == WGPURequestAdapterStatus_Success){
        data->adapter = adaptor;
    } else {
        String str = {(u8*)message.data, message.length};
        SysDebug(FAIL, str);
    }
}

static void HandleDeviceRequest(WGPURequestDeviceStatus status, WGPUDevice device,
                                    WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)userdata2;
    WindowData* data = (WindowData*)userdata1;
    if(status == WGPURequestDeviceStatus_Success){
        data->device = device;
    } else {
        String str = {(u8*)message.data, message.length};
        SysDebug(FAIL, str);
    }
}


ILIB_API IStatus SetupRenderer(){
    instance = wgpuCreateInstance(NULL);
    if(instance == NULL){
        SysDebug(FAIL, QSTR("couldn't set up WGPU"));
        return ISTATUS_WGPU_FAIL;
    }
    SysDebug(PASS, QSTR("WGPU has been started"));
    return ISTATUS_SUCCESS;
}


IStatus SetupRendererForWindow(WindowData* data){
    data->surface = (void*)SDL_GetWGPUSurface(instance, data->window);
    if(data->surface == NULL){
        SysDebug(FAIL, QSTR("couldn't create WGPU surface"));
        return ISTATUS_WGPU_FAIL;
    }
    WGPURequestAdapterOptions adaptorOptions = {0};
    adaptorOptions.compatibleSurface = data->surface;
    WGPURequestAdapterCallbackInfo adapCallback = {0};
    adapCallback.callback = HandleAdaptorRequest;
    adapCallback.userdata1 = data;
    wgpuInstanceRequestAdapter(instance, &adaptorOptions, adapCallback);
    if(data->adapter == NULL){
        SysDebug(FAIL, QSTR("couldn't create WGPU adaptor"));
        return ISTATUS_WGPU_FAIL;
    }
    WGPURequestDeviceCallbackInfo deviceCallback = {0};
    deviceCallback.callback = HandleDeviceRequest;
    deviceCallback.userdata1 = data;
    wgpuAdapterRequestDevice(data->adapter, NULL, deviceCallback);
    if(data->device == NULL){
        SysDebug(FAIL, QSTR("couldn't create WGPU device"));
        return ISTATUS_WGPU_FAIL;
    }
    data->queue = wgpuDeviceGetQueue(data->device);
    if(data->queue == NULL){
        SysDebug(FAIL, QSTR("couldn't get WGPU queue"));
        return ISTATUS_WGPU_FAIL;
    }
    String str = QSTR("pipeline_layout");
    WGPUPipelineLayoutDescriptor pLayoutDesc = {0};
    pLayoutDesc.label.data = (const char*)str.buffer;
    pLayoutDesc.label.length = str.length;
    data->pipelineLayout = wgpuDeviceCreatePipelineLayout(data->device, &pLayoutDesc);
    if(data->pipelineLayout == NULL){
        SysDebug(FAIL, QSTR("couldn't get WGPU pipeline layout"));
        return ISTATUS_WGPU_FAIL;
    }
    return ISTATUS_SUCCESS;
}

IStatus CleanupRendererForWindow(WindowData* data){
    wgpuPipelineLayoutRelease(data->pipelineLayout);
    wgpuQueueRelease(data->queue);
    wgpuDeviceRelease(data->device);
    wgpuAdapterRelease(data->adapter);
    wgpuSurfaceRelease(data->surface);
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus CleanupRenderer(){
    wgpuInstanceRelease(instance);;
    return ISTATUS_SUCCESS;
}