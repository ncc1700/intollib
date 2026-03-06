#include <intollib.h>





int main(){
    IntollibSetup();

    Window win = {0};
    IStatus status = InitWindow(&win, QSTR("Hello!"), 
        800, 600);
    if(!IS_SUCCESS(status)){
        return status;
    }
    while(WindowIsRunning(&win)){
        BeginDrawing(&win, RGB(0, 0, 0, 255));
        Color color;
        if(IsMouseDown(&win, MOUSE_BUTTON_LEFT)){
            color = RGB(255, 255, 0, 255);
        } else {
            color = RGB(0, 255, 255, 255);
        }
        IRectangle rect = {GetMouseX(&win), GetMouseY(&win), 100, 100};
        DrawRectangle(&win, rect, color);
        if(IsKeyDown(&win, 'h')){
            DrawString(&win, QSTR("Hello!"), 10, 10, 1, RGB(255, 255, 255, 255));
        } else {
            DrawString(&win, QSTR("Click 'h' on your keyboard!"), 
                        10, 10, 1, RGB(255, 255, 255, 255));
        }
        EndDrawing(&win);
    }
    DeleteWindow(&win);
    IntollibCleanup();
    
    return 0;
}