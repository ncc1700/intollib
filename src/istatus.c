#include <intollib.h>



static const char* IStatusToCStr[] = {
    [ISTATUS_SUCCESS] = "Operation completed successfully",
    [ISTATUS_UNKNOWN] = "Unknown IStatus",
    [ISTATUS_UNIMPLEMENTED] = "This is unimplemented",
    [ISTATUS_CANT_CREATE_HEAP] = "Cannot create heap",
    [ISTATUS_CANT_FREE] = "Cannot free memory",
    [ISTATUS_CANT_CLEANUP] = "Cannot cleanup system",
    [ISTATUS_OUT_OF_MEMORY] = "Out of memory",
    [ISTATUS_CANT_CREATE_WINDOW] = "Cannot create window",
    [ISTATUS_SDL_FAIL] = "SDL3 init failed",
    [ISTATUS_WGPU_FAIL] = "WGPU init failed",
    [ISTATUS_INVALID_PARAMETERS] = "Invalid parameters",
    [ISTATUS_CANT_CREATE_FILE] = "Cannot create file",
    [ISTATUS_ZERO_SIZE] = "The size is 0",
    [ISTATUS_CANT_READ] = "Cannot read contents",
    [ISTATUS_NOT_INITIALIZED] = "You haven't initialized intollib"
};


ILIB_API const char* IStatusToCString(IStatus status){
    int arrSize = ARR_LENGTH(IStatusToCStr);
    if(status >= arrSize) return IStatusToCStr[ISTATUS_UNKNOWN];
    return IStatusToCStr[status];
}

ILIB_API String IStatusToString(IStatus status){
    return QSTR(IStatusToCString(status));
}
