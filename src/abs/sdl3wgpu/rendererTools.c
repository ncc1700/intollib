#include "rendererTools.h"
#include "intollib.h"

static const char* backendStr[] = {
    [WGPUBackendType_Undefined] = "WGPU/Undefined",
    [WGPUBackendType_Null] = "WGPU/Unknown",
    [WGPUBackendType_WebGPU] = "WGPU/WebGPU",
    [WGPUBackendType_D3D11] = "WGPU/Direct3D 11",
    [WGPUBackendType_D3D12] = "WGPU/Direct3D 12",
    [WGPUBackendType_Metal] = "WGPU/Metal",
    [WGPUBackendType_Vulkan] = "WGPU/Vulkan",
    [WGPUBackendType_OpenGL] = "WGPU/OpenGL",
    [WGPUBackendType_OpenGLES] = "WGPU/OpenGL ES",
};

const char* WGPUBackendToCStr(WGPUBackendType backend){
    int arrSize = ARR_LENGTH(backendStr);
    if(arrSize <= backend){
        return backendStr[WGPUBackendType_Null];
    }
    return backendStr[backend];
}

String WGPUBackendToString(WGPUBackendType backend){
    return QSTR(WGPUBackendToCStr(backend));
}

String WGPUStringViewToIntollibString(WGPUStringView strView){
    String str = {(u8*)strView.data, strView.length};
    return str;
}

WGPUStringView IntollibStringToWGPUStringView(String str){
    WGPUStringView strView = {(const char*)str.buffer, str.length};
    return strView;
}

void PrintAdapterData(WGPUAdapter adapter){
    WGPUAdapterInfo info = {0};
    wgpuAdapterGetInfo(adapter, &info);
    SysPrint(QSTR("\n\n---------------------------------------------\n"));
    SysDebug(INFO, QSTR("GPU VENDER:"));
    SysDebug(UNKNOWN, WGPUStringViewToIntollibString(info.vendor));
    SysDebug(INFO, QSTR("DEVICE:"));
    SysDebug(UNKNOWN, WGPUStringViewToIntollibString(info.device));
    SysDebug(INFO, QSTR("BACKEND:"));
    SysDebug(UNKNOWN, WGPUBackendToString(info.backendType));
    SysPrint(QSTR("---------------------------------------------\n\n"));
}