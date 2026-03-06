#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "intollib.h"


static u8 keyMap[] = {
    [0] = '\0',
    [SDLK_RETURN] = '\n',
    [SDLK_ESCAPE] = 129,
    [SDLK_BACKSPACE] = '\b',
    [SDLK_TAB] = '\t',
    [SDLK_SPACE] =  ' ',
    [SDLK_EXCLAIM] = '!',
    [SDLK_DBLAPOSTROPHE] = '"',
    [SDLK_HASH] = '#',
    [SDLK_DOLLAR] = '$',
    [SDLK_PERCENT] = '%',
    [SDLK_AMPERSAND] = '&',
    [SDLK_APOSTROPHE] = '\'',
    [SDLK_LEFTPAREN] = '(',
    [SDLK_RIGHTPAREN] = ')',
    [SDLK_ASTERISK] = '*',
    [SDLK_PLUS] = '+',
    [SDLK_COMMA] = ',',
    [SDLK_MINUS] = '-',
    [SDLK_PERIOD] = '.',
    [SDLK_SLASH] = '/',
    [SDLK_0] =   '0',
    [SDLK_1] =   '1',
    [SDLK_2] =   '2',
    [SDLK_3] =   '3',
    [SDLK_4] =   '4',
    [SDLK_5] =   '5',
    [SDLK_6] =   '6',
    [SDLK_7] =   '7',
    [SDLK_8] =   '8',
    [SDLK_9] =   '9',
    [SDLK_COLON] =   ':',
    [SDLK_SEMICOLON] =   ';',
    [SDLK_LESS] =   '<',
    [SDLK_EQUALS] =   '=',
    [SDLK_GREATER] =   '>',
    [SDLK_QUESTION] =   '?',
    [SDLK_AT] =   '@',
    [SDLK_LEFTBRACKET] =   '[',
    [SDLK_BACKSLASH] =   '\\',
    [SDLK_RIGHTBRACKET] =   ']',
    [SDLK_CARET] =   '^',
    [SDLK_UNDERSCORE] =   '_',
    [SDLK_GRAVE] =   '`',
    [SDLK_A] =   'a',
    [SDLK_B] =   'b',
    [SDLK_C] =   'c',
    [SDLK_D] =   'd',
    [SDLK_E] =   'e',
    [SDLK_F] =   'f',
    [SDLK_G] =   'g',
    [SDLK_H] =   'h',
    [SDLK_I] =   'i',
    [SDLK_J] =   'j',
    [SDLK_K] =   'k',
    [SDLK_L] =   'l',
    [SDLK_M] =   'm',
    [SDLK_N] =   'n',
    [SDLK_O] =   'o',
    [SDLK_P] =   'p',
    [SDLK_Q] =   'q',
    [SDLK_R] =   'r',
    [SDLK_S] =   's',
    [SDLK_T] =   't',
    [SDLK_U] =   'u',
    [SDLK_V] =   'v',
    [SDLK_W] =   'w',
    [SDLK_X] =   'x',
    [SDLK_Y] =   'y',
    [SDLK_Z] =   'z',
    [SDLK_LEFTBRACE] =   '{',
    [SDLK_PIPE] =   '|',
    [SDLK_RIGHTBRACE] =   '}',
    [SDLK_TILDE] =   '~',
    [SDLK_DELETE] =   130,  
    [SDLK_PLUSMINUS] =   131,
};

static u8 mouseMap[] = {
    [0] = MOUSE_BUTTON_NONE,
    [SDL_BUTTON_LEFT] = MOUSE_BUTTON_LEFT,
    [SDL_BUTTON_MIDDLE] = MOUSE_BUTTON_MIDDLE,
    [SDL_BUTTON_RIGHT] = MOUSE_BUTTON_RIGHT
};





u8 KeyCodeToChar(u8 keyCode){
    int arrSize = ARR_LENGTH(keyMap);
    if(arrSize <= keyCode){
        return keyMap[0];
    }
    return keyMap[keyCode];
}

u8 SDLMouseToIntollibMouse(u8 sdlMouse){
    int arrSize = ARR_LENGTH(mouseMap);
    if(arrSize <= sdlMouse){
        return mouseMap[0];
    }
    return mouseMap[sdlMouse];
}