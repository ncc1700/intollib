#include <intollib.h>



ILIB_API u64 GetStringLength(u8* string){
    if(string == NULL) return 0;
    u8* copy = string;
    u64 length = 0;
    while(*copy != '\0'){
        length++;
        copy++;
    }
    return length;
}

ILIB_API u64 GetStringLengthEx(u8* string, bool addNewLine){
    if(string == NULL) return 0;
    u8* copy = string;
    u64 length = 0;
    while(*copy != '\0'){
        length++;
        copy++;
    }
    if(addNewLine == TRUE) return length + 1;
    else return length;
}

ILIB_API String InitStringEx(u8* str, u64 length){
    String qstr = {str, length};
    return qstr;
}

ILIB_API String InitString(u8* str){
    return InitStringEx(str, GetStringLength(str));
}

// ALLOCATES MEMORY FROM HEAP
ILIB_API char* QStringToCStringHeap(String str){
    char* s = AllocateMemory(str.length * sizeof(char));

    for(u64 i = 0; i < str.length; i++){
        if(i >= str.length) break;
        s[i] = str.buffer[i];
    }
    return s;
}

// ALLOCATES MEMORY FROM ARENA
ILIB_API char* QStringToCStringArena(Arena* arena, String str){
    char* s = PushMemoryFromArena(arena, str.length * sizeof(char));

    for(u64 i = 0; i < str.length; i++){
        if(i >= str.length) break;
        s[i] = str.buffer[i];
    }
    return s;
}

// just copies the pointer reference
ILIB_API void CopyString(String src, String* dest){
    dest->buffer = src.buffer;
    dest->length = src.length;
}

// dest.buffer is allocated from the heap, free it
ILIB_API bool CopyStringHeap(String src, String* dest){
    if(src.buffer == NULL) return FALSE;
    dest->buffer = AllocateMemory((src.length + 1) * sizeof(u8));
    if(dest->buffer == NULL) return FALSE;
    memcpy(dest->buffer, src.buffer, src.length * sizeof(u8));
    dest->buffer[src.length] = '\0'; 
    dest->length = src.length;
    return TRUE;
}

// dest.buffer is allocated from an arena, deal with it
ILIB_API bool CopyStringArena(Arena* arena, String src, String* dest){
    if(src.buffer == NULL) return FALSE;
    dest->buffer = PushMemoryFromArena(arena, (src.length + 1) * sizeof(u8));
    if(dest->buffer == NULL) return FALSE;
    memcpy(dest->buffer, src.buffer, src.length * sizeof(u8));
    dest->buffer[src.length] = '\0'; 
    dest->length = src.length;
    return TRUE;
}   

// just copies the pointer reference
ILIB_API String DupeString(String src){
    String result = {0};
    result.buffer = src.buffer;
    result.length = src.length;
    return result;
}

// result.buffer is allocated from the heap, free it
ILIB_API String DupeStringHeap(String src){
    String result = {0};
    if(src.buffer == NULL) goto EXIT;
    result.buffer = AllocateMemory((src.length + 1) * sizeof(u8));
    if(result.buffer == NULL) goto EXIT;
    memcpy(result.buffer, src.buffer, src.length * sizeof(u8));
    result.buffer[src.length] = '\0'; 
    result.length = src.length;
    EXIT:
    return result;
}

// result.buffer is allocated from an arena, deal with it
ILIB_API String DupeStringArena(Arena* arena, String src){
    String result = {0};
    if(src.buffer == NULL) goto EXIT;
    result.buffer = PushMemoryFromArena(arena, (src.length + 1) * sizeof(u8));
    if(result.buffer == NULL) goto EXIT;;
    memcpy(result.buffer, src.buffer, src.length * sizeof(u8));
    result.buffer[src.length] = '\0'; 
    result.length = src.length;
    EXIT:
    return result;
}   

ILIB_API String SliceString(String string, u64 length){
    String result = {0};
    if(string.buffer == NULL) goto EXIT;
    if(length >= string.length) return string;
    result.buffer = string.buffer; 
    result.length = length;
    EXIT:
    return result;
}


