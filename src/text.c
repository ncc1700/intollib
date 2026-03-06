#include "extern/BitFonts/font_8x12.h"
#include "intollib.h"




ILIB_API void DrawChar(Window* window, char c, u32 x, u32 y, u32 size, Color color){
    const u8* bmp = font_8x12[c];
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            if(bmp[i] & (1 << j)){
                IRectangle rect = {x + j * size, y + i * size, size, size};
                DrawRectangle(window, rect, color);
            }
        }
    }
}

ILIB_API void DrawString(Window* window, const char* s, u32 x, u32 y, u32 size, Color color){
    u32 currentX = x;
    u32 currentY = y;
    u32 fsize = 8;
    while(*s != '\0'){
        DrawChar(window, *s, currentX, currentY, size, color);
        currentX += size * fsize;
        s++;
    }
}