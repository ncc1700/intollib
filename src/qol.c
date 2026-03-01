#include <intollib.h>












Vec2d NDC2DToScreenSpace2D(Window* window, Vec2f ndc){
    Vec2d v2d = {0};
    v2d.x = ((double)ndc.x + 1.0) * 0.5 * GetWindowWidth(window);
    v2d.y = (1.0 - (double)ndc.y) * 0.5 * GetWindowHeight(window);
    return v2d;
}


Vec2f ScreenSpace2DToNDC2D(Window* window, Vec2d ss){
    Vec2f v2f = {0};
    v2f.x = ((2 * ss.x) / GetWindowWidth(window)) - 1;
    v2f.y = 1 - ((2 * ss.y) / GetWindowHeight(window));
    return v2f;
}