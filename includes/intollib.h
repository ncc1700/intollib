#ifndef INTOLLIB_H_INCLUDED
#define INTOLLIB_H_INCLUDED


#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef int8_t i8;

typedef uint16_t u16;
typedef int16_t i16;

typedef uint32_t u32;
typedef int32_t i32;

typedef uint64_t u64;
typedef int64_t i64;

typedef uintptr_t uptr;
typedef intptr_t iptr;

typedef uptr Handle;

#ifndef bool
typedef u8 bool
#endif

typedef enum _IStatus {
    ISTATUS_SUCCESS, ISTATUS_UNKNOWN, ISTATUS_UNIMPLEMENTED,
    ISTATUS_CANT_CREATE_HEAP, ISTATUS_CANT_FREE, ISTATUS_CANT_CLEANUP,
    ISTATUS_OUT_OF_MEMORY, ISTATUS_CANT_CREATE_WINDOW, ISTATUS_SDL_FAIL,
    ISTATUS_WGPU_FAIL, ISTATUS_INVALID_PARAMETERS, ISTATUS_CANT_CREATE_FILE,
    ISTATUS_ZERO_SIZE, ISTATUS_CANT_READ
} IStatus;
typedef enum _DebugType {
    PASS, FAIL, WARNING, UNKNOWN, INFO
} DebugType;

typedef struct _Vec2f {
    float x;
    float y;
} Vec2f;

typedef struct _Vec2d {
    double x;
    double y;
} Vec2d;

typedef struct _Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct _Vec3d {
    double x;
    double y;
    double z;
} Vec3d;

#define FALSE 0
#define TRUE 1

#ifdef _WIN32
#define ILIB_API __declspec(dllexport)
#endif

#define ARR_LENGTH(x) sizeof(x)/sizeof(x[0]);
#define IS_SUCCESS(x) (x == ISTATUS_SUCCESS)



// Setup
ILIB_API IStatus IntollibSetup();
ILIB_API IStatus IntollibCleanup();

// Memory API
// some taken from my OS, nearlands
#define KB(x) ((u64)(x) << 10)
#define MB(x) ((u64)(x) << 20)
#define GB(x) ((u64)(x) << 30)
#define TB(x) ((u64)(x) << 40)

#define TO_KB(x) ((u64)(x) >> 10)
#define TO_MB(x) ((u64)(x) >> 20)
#define TO_GB(x) ((u64)(x) >> 30)
#define TO_TB(x) ((u64)(x) >> 40)

typedef struct _Arena {
    void* base;
    u64 size;
    u64 used;
} Arena;

ILIB_API IStatus SetupMemoryAllocator();
ILIB_API void* AllocateMemory(u64 allocSize);
ILIB_API IStatus FreeMemory(void* address);
ILIB_API IStatus CleanupMemoryAllocator();
ILIB_API IStatus CreateArena(Arena* arena, u64 arenaSize);
ILIB_API void* PushMemoryFromArena(Arena* arena, u64 allocSize);
ILIB_API IStatus PopMemoryFromArena(Arena* arena, u64 popSize);
ILIB_API IStatus ClearArena(Arena* arena);
ILIB_API IStatus DestroyArena(Arena* arena);
void *memcpy(void* dest, const void* src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// String API
// taken from my OS, nearlands
typedef struct _String {
    u8* buffer;
    u64 length;
} String;

#define Q(x) (u8*)x
#define C(x) (char*)x
#define QSTR(x) InitString(Q(x))

ILIB_API u64 GetStringLength(u8* string);
ILIB_API u64 GetStringLengthEx(u8* string, bool addNewLine);
ILIB_API String InitStringEx(u8* str, u64 length);
ILIB_API String InitString(u8* str);
ILIB_API char* QStringToCStringHeap(String str);
ILIB_API char* QStringToCStringArena(Arena* arena, String str);
ILIB_API void CopyString(String src, String* dest);
ILIB_API bool CopyStringHeap(String src, String* dest);
ILIB_API bool CopyStringArena(Arena* arena, String src, String* dest);
ILIB_API String DupeString(String src);
ILIB_API String DupeStringHeap(String src);
ILIB_API String DupeStringArena(Arena* arena, String src);
ILIB_API String SliceString(String string, u64 length);

// IStatus stuff
ILIB_API const char* IStatusToCString(IStatus status);
ILIB_API String IStatusToString(IStatus status);

// Console API
ILIB_API void SysPrintCStr(const char* cstr);
ILIB_API void SysPrint(String str);
ILIB_API void SysDebug(DebugType type, String str, ...);

// File API
typedef enum _FileOpenType {
    FILE_OPEN,
    FILE_CREATE_ALWAYS,
    FILE_CREATE_IF_NOT_EXIST
} FileOpenType;

ILIB_API IStatus FileOpen(Handle* handle, String fileName, char access, FileOpenType type);
ILIB_API IStatus FileReadHeap(Handle file, String* str);
ILIB_API IStatus FileReadArena(Arena* arena, Handle file, String* str);
ILIB_API IStatus FileClose(Handle file);

// Window API
typedef struct _Window {
    void* winData;
    void* renderer;
    Arena arena;
    u64 curKey;
    u8 curMouse;
    u32 width;
    u32 height;
    bool isRunning;
} Window;

ILIB_API IStatus SetupWindow();
ILIB_API IStatus InitWindow(Window* window, String title, u32 width, u32 height);
ILIB_API bool WindowIsRunning(Window* window);
ILIB_API u32 GetWindowWidth(Window* window);
ILIB_API u32 GetWindowHeight(Window* window);
ILIB_API IStatus DeleteWindow(Window* window);
ILIB_API IStatus CleanupWindow();

// Renderer API
typedef struct _Colorf {
    float r;
    float g;
    float b;
    float a;
} Colorf;
#define RGBF(r, g, b, a) ((Colorf){r, g, b, a})
ILIB_API IStatus SetupRenderer();
ILIB_API void BeginDrawing(Window* window, Colorf backgroundColor);
ILIB_API void DrawHelloTriangle(Window* window);
//ILIB_API void ClearBackground(Window* window, Colorf color);
ILIB_API void EndDrawing(Window* window);
ILIB_API IStatus CleanupRenderer();

// Quality of Life
Vec2d NDC2DToScreenSpace2D(Window* window, Vec2f ndc);
Vec2f ScreenSpace2DToNDC2D(Window* window, Vec2d ss);
#endif