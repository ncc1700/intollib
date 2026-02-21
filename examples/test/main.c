#include <intollib.h>





int main(){
    IntollibSetup();

    Window win = {0};
    InitWindow(&win, QSTR("Hello!"), 800, 600);
    while(WindowIsRunning(&win)){
        BeginDrawing(&win);

        EndDrawing(&win);
    }
    DeleteWindow(&win);
    IntollibCleanup();
    
    return 0;
}