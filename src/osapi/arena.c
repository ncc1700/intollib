#include <intollib.h>





ILIB_API IStatus CreateArena(Arena* arena, u64 size){
    arena->size = size;
    arena->base = AllocateMemory(arena->size);
    if(arena->base == NULL){
        return ISTATUS_OUT_OF_MEMORY;
    }
    arena->used = 0;
    return ISTATUS_SUCCESS;
}


ILIB_API void* PushMemoryFromArena(Arena* arena, u64 size){
    if(arena->base == NULL){
        return NULL;
    }
    if((arena->used + size) > arena->size){
        return NULL;
    }
    u64 usedBeforeAdd = arena->used;
    arena->used += size;
    return (void*)((u8*)arena->base + usedBeforeAdd);
}


ILIB_API IStatus PopMemoryFromArena(Arena* arena, u64 size){
    if(size >= arena->used) arena->used = 0;
    else arena->used -= size;
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus ClearArena(Arena* arena){
    arena->used = 0;
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus DestroyArena(Arena* arena){
    IStatus result = FreeMemory(arena->base);
    if(result == FALSE) return FALSE;
    arena->size = 0;
    arena->base = 0;
    return result;
}