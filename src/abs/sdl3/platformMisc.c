#include "intollib.h"










ILIB_API u8 GetKey(Window* window){
    return window->curKey;
}

ILIB_API bool IsKeyDown(Window* window, u8 key){
    if(window->curKey == key) return true;
    else return false;
}

ILIB_API Vec2 GetMousePosition(Window* window){
    return (Vec2){window->curMouse.x, window->curMouse.y};
}

ILIB_API u64 GetMouseX(Window* window){
    return window->curMouse.x;
}

ILIB_API u64 GetMouseY(Window* window){
    return window->curMouse.y;
}

ILIB_API bool IsMouseDown(Window* window, u64 button){
    if(window->curMouse.button == button) return true;
    else return false;
}
