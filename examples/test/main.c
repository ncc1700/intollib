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
        BeginDrawing(&win, RGBF(0.0, 0.3, 0.3, 1.0));
        DrawHelloTriangle(&win);
        EndDrawing(&win);
    }
    DeleteWindow(&win);
    IntollibCleanup();
    
    return 0;
}