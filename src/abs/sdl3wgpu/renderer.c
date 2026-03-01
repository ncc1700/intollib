#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "extern/sdl3webgpu.h"
#include "intollib.h"
#include "sdl3wgpu.h"
#include "webgpu/webgpu.h"
#include <math.h>

typedef struct _VertexData {
    float scale;
    float _pad[3];
    float position[4];
    float _pad2[3];
} VertexData;

static WGPUInstance instance = {0};

static const char* defaultShader = 
    "@vertex\n"
    "fn VertexShaderMain(@builtin(vertex_index) inVertexIndex: u32) -> @builtin(position) vec4<f32> {\n"
        "let x = f32(i32(inVertexIndex) - 1);\n"
        "let y = f32(i32(inVertexIndex & 1u) * 2 - 1);\n"
        "return vec4<f32>(x, y, 0.0, 1.0);"
    "}\n"
    "@fragment\n"
    "fn FragmentShaderMain() -> @location(0) vec4<f32> {\n"
        "return vec4<f32>(1.0, 1.0, 0.0, 1.0);\n"
    "}\n";
    


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

WGPUShaderModule LoadShader(WGPUDevice device, String name, String shaderCode){
    WGPUShaderModuleDescriptor shaderModDesc = {0}; 
    
    shaderModDesc.label.data = (const char*)name.buffer;
    shaderModDesc.label.length = name.length;
    WGPUChainedStruct chainedStr = {0};
    WGPUShaderSourceWGSL shaderSrcWGSL = {0};
    shaderSrcWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
    
    shaderSrcWGSL.code.data = (const char*)shaderCode.buffer;
    shaderSrcWGSL.code.length = shaderCode.length;
    shaderModDesc.nextInChain = (const WGPUChainedStruct *)&shaderSrcWGSL;
    return wgpuDeviceCreateShaderModule(device, &shaderModDesc);
}

WGPUShaderModule LoadShaderFromFile(WindowData* data, String fileName){
    Handle file = 0;
    IStatus status = FileOpen(&file, fileName, 'r', FILE_OPEN);
    if(!IS_SUCCESS(status)){
        return NULL;
    }
    String fileContent = {0};
    status = FileReadArena(data->arena, file, &fileContent);
    if(!IS_SUCCESS(status)){
        // wish defer existed in C =(
        FileClose(file);
        return NULL;
    }
    
    FileClose(file);
    return LoadShader(data->device, fileName, fileContent);
}

static IStatus SetupAdaptor(WindowData* data){
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
    return ISTATUS_SUCCESS;
}

static IStatus SetupDevice(WindowData* data){
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
    return ISTATUS_SUCCESS;
}

static WGPUBuffer CreateUniformBuffer(WindowData* data, u64 size){
    WGPUBufferDescriptor bufDesc = {0};
    bufDesc.size = size;
    bufDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    WGPUBuffer buffer = wgpuDeviceCreateBuffer(data->device, &bufDesc);
    return buffer;
}

static WGPUBindGroupLayoutEntry CreateUniformBindGroupLayout(WindowData* data, WGPUShaderStage visibility, u32 binding, u64 size){
    WGPUBindGroupLayoutEntry bglEntry = {0};
    bglEntry.binding = binding;
    bglEntry.visibility = visibility;
    bglEntry.buffer.type = WGPUBufferBindingType_Uniform;
    bglEntry.buffer.minBindingSize = size;
    return bglEntry;
}

static WGPUBindGroupEntry CreateUniformBindGroup(WGPUBuffer buffer, u32 binding, u32 offset, u64 size){
    WGPUBindGroupEntry bgEntry = {0};
    bgEntry.binding = binding;
    bgEntry.buffer = buffer;
    bgEntry.offset = offset;
    bgEntry.size = size;
    return bgEntry;
}

static IStatus SetupRenderPipelineLayout(WindowData* data){

    // color uniform setup
    data->colorBuf = CreateUniformBuffer(data, sizeof(float) * 4);
    if(data->colorBuf == NULL){
        SysDebug(FAIL, QSTR("Couldn't create color buffer"));
        return ISTATUS_WGPU_FAIL;
    }

    WGPUBindGroupLayoutEntry cbglEntry = CreateUniformBindGroupLayout(data, 
        WGPUShaderStage_Fragment | WGPUShaderStage_Vertex, 
        0, sizeof(float) * 4);
    
    WGPUBindGroupEntry cbgEntry = CreateUniformBindGroup(data->colorBuf, 0, 0, sizeof(float) * 4);

    // vertex data setup
    data->scaleBuf = CreateUniformBuffer(data, sizeof(VertexData));
    if(data->scaleBuf == NULL){
        SysDebug(FAIL, QSTR("Couldn't create scale buffer"));
        return ISTATUS_WGPU_FAIL;
    }

    WGPUBindGroupLayoutEntry sbglEntry = CreateUniformBindGroupLayout(data, 
        WGPUShaderStage_Vertex, 
        1, sizeof(VertexData));
    
    WGPUBindGroupEntry sbgEntry = CreateUniformBindGroup(data->scaleBuf, 1, 0, sizeof(VertexData));

    // arrays we store the uniforms
    WGPUBindGroupLayoutEntry bglEntries[] = {cbglEntry, sbglEntry};
    u16 bglEntriesSize = ARR_LENGTH(bglEntries);
    WGPUBindGroupEntry bgEntries[] = {cbgEntry, sbgEntry};
    u16 bgEntriesSize = ARR_LENGTH(bgEntries);
    WGPUBindGroupLayoutDescriptor bLayoutDesc = {0};
    bLayoutDesc.entries = bglEntries;
    bLayoutDesc.entryCount = bglEntriesSize; // use arr_length next time if it works
    data->bGroupLayout = wgpuDeviceCreateBindGroupLayout(data->device, &bLayoutDesc);
    if(data->bGroupLayout == NULL){
        SysDebug(FAIL, QSTR("Couldn't create bind group layout"));
        return ISTATUS_WGPU_FAIL;
    }
    WGPUBindGroupLayout bfLayoutDesc[] = {data->bGroupLayout};
    String pipelineStr = QSTR("pipeline_layout");
    WGPUPipelineLayoutDescriptor pLayoutDesc = {0};
    pLayoutDesc.label.data = (const char*)pipelineStr.buffer;
    pLayoutDesc.label.length = pipelineStr.length;
    pLayoutDesc.bindGroupLayoutCount = 1;
    pLayoutDesc.bindGroupLayouts = bfLayoutDesc;
    data->pipelineLayout = wgpuDeviceCreatePipelineLayout(data->device, &pLayoutDesc);
    if(data->pipelineLayout == NULL){
        SysDebug(FAIL, QSTR("couldn't get WGPU pipeline layout"));
        return ISTATUS_WGPU_FAIL;
    }
    WGPUBindGroupDescriptor bgDesc = {0};
    bgDesc.entries = bgEntries;
    bgDesc.entryCount = bgEntriesSize;
    bgDesc.layout = data->bGroupLayout;
    data->bindGroup = wgpuDeviceCreateBindGroup(data->device, &bgDesc);
    return ISTATUS_SUCCESS;
}

static IStatus SetupRenderPipeline(WindowData* data){
     wgpuSurfaceGetCapabilities(data->surface, data->adapter, &data->surfaceCapablities);
    WGPURenderPipelineDescriptor desc = {0};
    String rpStr = QSTR("render_pipeline");
    desc.label.data = (const char*)rpStr.buffer;
    desc.label.length = rpStr.length;
    String vertexEntryPoint = QSTR("VertexShaderMain");
    WGPUVertexState vState = {0};
    vState.entryPoint.data = (const char*)vertexEntryPoint.buffer;
    vState.entryPoint.length = vertexEntryPoint.length;
    vState.module = data->shaderMod;

    String fragmentEntryPoint = QSTR("FragmentShaderMain");
    WGPUFragmentState fState = {0};
    fState.entryPoint.data = (const char*)fragmentEntryPoint.buffer;
    fState.entryPoint.length = fragmentEntryPoint.length;
    fState.module = data->shaderMod;
    fState.targetCount = 1;
    WGPUColorTargetState ctState = {0};
    ctState.format = data->surfaceCapablities.formats[0];
    ctState.writeMask = WGPUColorWriteMask_All;
    WGPUColorTargetState ctStates[] = {ctState};
    fState.targets = ctStates;

    WGPUPrimitiveState primState = {0};
    primState.topology = WGPUPrimitiveTopology_TriangleList;

    WGPUMultisampleState mSampleState = {0};
    mSampleState.count = 1;
    mSampleState.mask = 0xFFFFFFFF;

    desc.layout = data->pipelineLayout;
    desc.vertex = vState;
    desc.fragment = &fState;
    desc.primitive = primState;
    desc.multisample = mSampleState;

    data->renderPipeline = wgpuDeviceCreateRenderPipeline(data->device, &desc);
    if(data->renderPipeline == NULL){
        SysDebug(FAIL, QSTR("couldn't create render pipeline"));
        return ISTATUS_WGPU_FAIL;
    }
    return ISTATUS_SUCCESS;
}

IStatus SetupRendererForWindow(WindowData* data){
    data->surface = (void*)SDL_GetWGPUSurface(instance, data->window);
    if(data->surface == NULL){
        SysDebug(FAIL, QSTR("couldn't create WGPU surface"));
        return ISTATUS_WGPU_FAIL;
    }
    IStatus status = SetupAdaptor(data);
    if(!IS_SUCCESS(status)){
        return status;
    }
    status = SetupDevice(data);
    if(!IS_SUCCESS(status)){
        return status;
    }

    status = SetupRenderPipelineLayout(data);
    if(!IS_SUCCESS(status)){
        return status;
    }

    //data->shaderMod = LoadShader(data->device, QSTR("shader"), QSTR(defaultShader));
    data->shaderMod = LoadShaderFromFile(data, QSTR("shader.wgsl"));
    if(data->shaderMod == NULL){
        SysDebug(FAIL, QSTR("couldn't compile WGPU shader"));
        return ISTATUS_WGPU_FAIL;
    }

    status = SetupRenderPipeline(data);
    if(!IS_SUCCESS(status)){
        return status;
    }

   
    
    data->surfaceConfig.device = data->device;
    data->surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    data->surfaceConfig.format = data->surfaceCapablities.formats[0];
    data->surfaceConfig.presentMode = WGPUPresentMode_Fifo;
    data->surfaceConfig.alphaMode = data->surfaceCapablities.alphaModes[0];
    SDL_GetWindowSize(data->window, (int*)&data->surfaceConfig.width, 
                                            (int*)&data->surfaceConfig.height);
    wgpuSurfaceConfigure(data->surface, &data->surfaceConfig);
    
    data->doStuffInEndDrawing = TRUE;
    return ISTATUS_SUCCESS;
}

static inline void ReconfigureSurface(WGPUSurfaceTexture* texture, WindowData* data){
    if(texture->texture != NULL){
        wgpuTextureRelease(texture->texture);
    }
    SDL_GetWindowSize(data->window, (int*)&data->surfaceConfig.width, 
                                            (int*)&data->surfaceConfig.height);
    wgpuSurfaceConfigure(data->surface, &data->surfaceConfig);                                   
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
    
    wgpuSurfaceGetCurrentTexture(data->surface, &data->surfaceText);
    switch(data->surfaceText.status){
        case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal:
        case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal:
            break;
        case WGPUSurfaceGetCurrentTextureStatus_Timeout:
        case WGPUSurfaceGetCurrentTextureStatus_Outdated:
        case WGPUSurfaceGetCurrentTextureStatus_Lost:
            ReconfigureSurface(&data->surfaceText, data);
            data->doStuffInEndDrawing = FALSE;
            return;
        case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
        case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
        case WGPUSurfaceGetCurrentTextureStatus_Error:
        case WGPUSurfaceGetCurrentTextureStatus_Force32:
            SysDebug(FAIL, QSTR("GetCurrentTexture FAILED withs status of %d\n"),  data->surfaceText.status);
            window->isRunning = FALSE;
            data->doStuffInEndDrawing = FALSE;
            break;
    }
    if(data->surfaceText.texture == NULL){
        SysDebug(FAIL, QSTR("Texture is NULL\n"));
        window->isRunning = FALSE;
        data->doStuffInEndDrawing = FALSE;
        return;
    }
    data->frame = wgpuTextureCreateView(data->surfaceText.texture, NULL);
    if(data->frame == NULL){
        SysDebug(FAIL, QSTR("Frame is NULL\n"));
        window->isRunning = FALSE;
        data->doStuffInEndDrawing = FALSE;
        return;
    }
    WGPUCommandEncoderDescriptor comDesc = {0};
    String comDescLabel = QSTR("command_encoder");
    comDesc.label.data = (const char*)comDescLabel.buffer;
    comDesc.label.length = comDescLabel.length;
    data->commandEncoder = wgpuDeviceCreateCommandEncoder(data->device, &comDesc);
    if(data->commandEncoder == NULL){
        SysDebug(FAIL, QSTR("commandEncoder is NULL\n"));
        window->isRunning = FALSE;
        data->doStuffInEndDrawing = FALSE;
        return;
    }
    WGPURenderPassDescriptor rpDesc = {0};
    String rpLabel = QSTR("render_pass_encoder");
    rpDesc.label.data = (const char*)rpLabel.buffer;
    rpDesc.label.length = rpLabel.length;
    rpDesc.colorAttachmentCount = 1;
    WGPURenderPassColorAttachment rpcAttach = {0};
    rpcAttach.view = data->frame;
    rpcAttach.loadOp = WGPULoadOp_Clear;
    rpcAttach.storeOp = WGPUStoreOp_Store;
    rpcAttach.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    rpcAttach.clearValue = (const WGPUColor){backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a};
    WGPURenderPassColorAttachment rpcAttachs[] = {rpcAttach};
    rpDesc.colorAttachments = rpcAttachs;
    data->renderPassEncoder = wgpuCommandEncoderBeginRenderPass(data->commandEncoder, &rpDesc);
    if(data->renderPassEncoder == NULL){
        SysDebug(FAIL, QSTR("rpEncoder is NULL\n"));
        window->isRunning = FALSE;
        data->doStuffInEndDrawing = FALSE;
        return;
    }

    // the actual drawing
    
    // the actual drawing (ended)
    
}   


ILIB_API void DrawHelloTriangle(Window* window){
    static int prev = 0;
    if(prev == 0){
        prev = SDL_GetTicks();
    }
    static float scale = 0.5;;
    if(SDL_GetTicks() - prev >= 10){
        scale += 0.01;
        prev = SDL_GetTicks();
    }
    WindowData* data = (WindowData*)window->winData;
    float color[4] = {1.0, 1.0, 1.0, 1.0};
    VertexData vData = {0};
    vData.scale = scale;
    vData.position[0] = 0.8;
    vData.position[1] = 1.0;
    wgpuQueueWriteBuffer(data->queue, data->colorBuf, 
                0, color, sizeof(color));
    wgpuQueueWriteBuffer(data->queue, data->scaleBuf, 
                0, &vData, sizeof(VertexData));
    
    
    wgpuRenderPassEncoderSetPipeline(data->renderPassEncoder, data->renderPipeline);
    wgpuRenderPassEncoderSetBindGroup(data->renderPassEncoder, 0, data->bindGroup, 0, NULL);
    wgpuRenderPassEncoderDraw(data->renderPassEncoder, 6, 1, 0, 0);
}

ILIB_API void DrawRectangle(Window* window, Vec2d position){
    static int prev = 0;
    if(prev == 0){
        prev = SDL_GetTicks();
    }
    static float scale = 0.5;;
    if(SDL_GetTicks() - prev >= 10){
        scale += 0.01;
        prev = SDL_GetTicks();
    }
    
    WindowData* data = (WindowData*)window->winData;
    float color[4] = {1.0, 1.0, 1.0, 1.0};
    float pos[4] = {1.0, 1.0, 0.0, 0.0};
    VertexData vData = {0};
    vData.scale = scale;
    vData.position[0] = 1.0;
    vData.position[1] = 1.0;
    
    wgpuQueueWriteBuffer(data->queue, data->colorBuf, 
                0, color, sizeof(color));
    wgpuQueueWriteBuffer(data->queue, data->scaleBuf, 
                0, &vData, sizeof(VertexData));
    
    
    wgpuRenderPassEncoderSetPipeline(data->renderPassEncoder, data->renderPipeline);
    wgpuRenderPassEncoderSetBindGroup(data->renderPassEncoder, 0, data->bindGroup, 0, NULL);
    wgpuRenderPassEncoderDraw(data->renderPassEncoder, 6, 1, 0, 0);
}

// ILIB_API void ClearBackground(Window* window, Colorf color){
//     WindowData* data = (WindowData*)window->winData;
//     WGPURenderPassDescriptor rpDesc = {0};
//     String rpLabel = QSTR("render_pass_encoder");
//     rpDesc.label.data = (const char*)rpLabel.buffer;
//     rpDesc.label.length = rpLabel.length;
//     rpDesc.colorAttachmentCount = 1;
//     WGPURenderPassColorAttachment rpcAttach = {0};
//     rpcAttach.view = data->frame;
//     rpcAttach.loadOp = WGPULoadOp_Clear;
//     rpcAttach.storeOp = WGPUStoreOp_Store;
//     rpcAttach.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
//     rpcAttach.clearValue = (const WGPUColor){color.r, color.g, color.b, color.a};
//     WGPURenderPassColorAttachment rpcAttachs[] = {rpcAttach};
//     rpDesc.colorAttachments = rpcAttachs;
//     WGPURenderPassEncoder rpEncoder = wgpuCommandEncoderBeginRenderPass(data->commandEncoder, &rpDesc);
//     if(rpEncoder == NULL){
//         SysDebug(FAIL, QSTR("rpEncoder is NULL\n"));
//         window->isRunning = FALSE;
//         data->doStuffInEndDrawing = FALSE;
//         return;
//     }
//     wgpuRenderPassEncoderSetPipeline(rpEncoder, data->renderPipeline);
//     wgpuRenderPassEncoderDraw(rpEncoder, 0, 1, 0, 0);
//     wgpuRenderPassEncoderEnd(rpEncoder);
//     wgpuRenderPassEncoderRelease(rpEncoder);
// }

ILIB_API void EndDrawing(Window* window){
    
    WindowData* data = (WindowData*)window->winData;
    if(data->doStuffInEndDrawing == FALSE){
        data->doStuffInEndDrawing = TRUE;
        return;
    }
    wgpuRenderPassEncoderEnd(data->renderPassEncoder);
    wgpuRenderPassEncoderRelease(data->renderPassEncoder);
    WGPUCommandBufferDescriptor cbDesc = {0};
    String cbLabel = QSTR("command_buffer");
    cbDesc.label.data = (const char*)cbLabel.buffer;
    cbDesc.label.length = cbLabel.length;
    data->cBuffer = wgpuCommandEncoderFinish(data->commandEncoder, &cbDesc);
    if(data->cBuffer == NULL){
        SysDebug(FAIL, QSTR("cBuffer is NULL\n"));
        window->isRunning = FALSE;
        data->doStuffInEndDrawing = FALSE;
        return;
    }

    WGPUCommandBuffer cBufs[] = {data->cBuffer};
    wgpuQueueSubmit(data->queue, 1, cBufs);
    wgpuSurfacePresent(data->surface);

    wgpuCommandBufferRelease(data->cBuffer);
    wgpuCommandEncoderRelease(data->commandEncoder);
    wgpuTextureViewRelease(data->frame);
    wgpuTextureRelease(data->surfaceText.texture);
    return;
}

IStatus CleanupRendererForWindow(WindowData* data){
    wgpuRenderPipelineRelease(data->renderPipeline);
    wgpuPipelineLayoutRelease(data->pipelineLayout);
    wgpuShaderModuleRelease(data->shaderMod);
    wgpuSurfaceCapabilitiesFreeMembers(data->surfaceCapablities);
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