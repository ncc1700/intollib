#include "intollib.h"
#include <windows.h>






ILIB_API IStatus FileOpen(Handle* handle, String fileName, char access, FileOpenType type){
    if(handle == NULL) return ISTATUS_INVALID_PARAMETERS;
    DWORD desiredAccess = 0;
    switch(access){
        case 'w':
            desiredAccess = GENERIC_WRITE;
            break;
        case 'r':
            desiredAccess = GENERIC_READ;
            break;
        case 'q':
            desiredAccess = GENERIC_READ | GENERIC_WRITE;
            break;
        case 'e':
            desiredAccess = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
            break;
        default:
            return ISTATUS_INVALID_PARAMETERS;
    }
    DWORD creationDisposition = 0;
    switch(type){   
        case FILE_OPEN:
            creationDisposition = OPEN_EXISTING;
            break;
        case FILE_CREATE_ALWAYS:
            creationDisposition = CREATE_ALWAYS;
            break;
        case FILE_CREATE_IF_NOT_EXIST:
            creationDisposition = CREATE_NEW;
            break;
        default:
            return ISTATUS_INVALID_PARAMETERS;
    }
    HANDLE hFile = CreateFileA((const char*)fileName.buffer, desiredAccess, 0, 
                            NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if(hFile == INVALID_HANDLE_VALUE){
        return ISTATUS_CANT_CREATE_FILE;
    }
    *handle = (Handle)hFile;
    return ISTATUS_SUCCESS;
}

// ALLOCATES MEMORY, PLEASE FREE
ILIB_API IStatus FileReadHeap(Handle file, String* str){
    if(file == 0) return ISTATUS_INVALID_PARAMETERS;
    DWORD size = GetFileSize((HANDLE)file, NULL);
    if(size == 0){
        return ISTATUS_ZERO_SIZE;
    }
    str->buffer = AllocateMemory(size + 1);
    if(str->buffer == NULL){
        return ISTATUS_OUT_OF_MEMORY;
    }
    BOOL result = ReadFile((HANDLE)file, str->buffer, 
                        size, 
                        (LPDWORD)&str->length, 
                        NULL);
    if(!result){
        // just free the buffer so we don't need to worry about the user
        // doing goofyness on it
        FreeMemory(str->buffer);
        return ISTATUS_CANT_READ;
    }
    str->buffer[str->length] = '\0';
    return ISTATUS_SUCCESS;
}

ILIB_API IStatus FileReadArena(Arena* arena, Handle file, String* str){
    if(file == 0) return ISTATUS_INVALID_PARAMETERS;
    DWORD size = GetFileSize((HANDLE)file, NULL);
    if(size == 0){
        return ISTATUS_ZERO_SIZE;
    }
    str->buffer = PushMemoryFromArena(arena, size + 1);
    if(str->buffer == NULL){
        return ISTATUS_OUT_OF_MEMORY;
    }
    BOOL result = ReadFile((HANDLE)file, str->buffer, 
                        size, 
                        (LPDWORD)&str->length, 
                        NULL);
    if(!result){
        // just free the buffer so we don't need to worry about the user
        // doing goofyness on it
        // idk if this is a good idea on an arena but oh well lol
        PopMemoryFromArena(arena, size + 1);
        return ISTATUS_CANT_READ;
    }
    str->buffer[str->length] = '\0';
    return ISTATUS_SUCCESS;
}


ILIB_API IStatus FileClose(Handle file){
    CloseHandle((HANDLE)file);
    return ISTATUS_SUCCESS;
}