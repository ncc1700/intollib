#include <Windows.h>
#include <intollib.h>

static HANDLE hHeap = NULL;
static CRITICAL_SECTION memCritSec = {0}; 

ILIB_API IStatus SetupMemoryAllocator(){
    InitializeCriticalSection(&memCritSec);
    IStatus status = ISTATUS_SUCCESS;
    hHeap = HeapCreate(0, 0, 0);
    if(hHeap == NULL){
        return ISTATUS_CANT_CREATE_HEAP;
    }
    return ISTATUS_SUCCESS;
}
ILIB_API void* AllocateMemory(u64 allocSize){
    EnterCriticalSection(&memCritSec);
    void* mem = NULL;
    if(hHeap == NULL){
        goto EXIT;
    }
    mem = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, allocSize);
EXIT:
    LeaveCriticalSection(&memCritSec);
    return mem;
}

ILIB_API IStatus FreeMemory(void* address){
    EnterCriticalSection(&memCritSec);
    IStatus status = ISTATUS_SUCCESS;
    bool result = HeapFree(hHeap, 0, address);
    if(result != TRUE){
        status = ISTATUS_CANT_FREE;
        goto EXIT;
    }
EXIT:
    LeaveCriticalSection(&memCritSec);
    return status;
}

ILIB_API IStatus CleanupMemoryAllocator(){
    DeleteCriticalSection(&memCritSec);
    bool result = HeapDestroy(hHeap);
    if(result != TRUE){
        return ISTATUS_CANT_CLEANUP;
    }
    return ISTATUS_SUCCESS;
}