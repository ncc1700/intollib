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
        IRectangle rect = {90, 90, 100, 100};
        DrawRectangle(&win, rect, RGB(0, 255, 255, 255));
        DrawString(&win, "Hello!", 10, 10, 1, RGB(255, 255, 255, 255));
        EndDrawing(&win);
    }
    DeleteWindow(&win);
    IntollibCleanup();
    
    return 0;
}