#include <intollib.h>






#include <intollib.h>







ILIB_API IStatus IntollibSetup(){
    IStatus status = SetupMemoryAllocator();
    if(!IS_SUCCESS(status)) return status;
    status = SetupWindow();
    return status;
}


ILIB_API IStatus IntollibCleanup(){
    IStatus status = CleanupMemoryAllocator();
    if(!IS_SUCCESS(status)) return status;
    status = CleanupWindow();
    return status;
}