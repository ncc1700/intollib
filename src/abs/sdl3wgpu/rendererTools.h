#ifndef RENDERERTOOLS_H_INCLUDED
#define RENDERERTOOLS_H_INCLUDED






#include <intollib.h>
#include "webgpu/webgpu.h"


#define WGPU_STRVIEW(x) ((WGPUStringView){.data=x, .length=GetStringLength((u8*)x)})

const char* WGPUBackendToCStr(WGPUBackendType backend);
String WGPUBackendToString(WGPUBackendType backend);
String WGPUStringViewToIntollibString(WGPUStringView strView);
WGPUStringView IntollibStringToWGPUStringView(String str);
void PrintAdapterData(WGPUAdapter adapter);


















#endif