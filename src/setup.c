#include <intollib.h>






#include <intollib.h>







ILIB_API IStatus IntollibSetup(){
    SysPrintCStr("\n!!!  starting intollib  !!!\n");
    IStatus status = SetupMemoryAllocator();
    if(!IS_SUCCESS(status)){
        SysPrintCStr("[FAIL] memory allocator setup FAILED\n");
        return status;
    }
    SysDebug(PASS, QSTR("memory allocator setup finished"));
    status = SetupRenderer();
    if(!IS_SUCCESS(status)){
        SysDebug(FAIL, QSTR("renderer setup FAILED: %d"), status);
        return status;
    }
    SysDebug(PASS, QSTR("renderer setup finished"));
    status = SetupWindow();
    if(!IS_SUCCESS(status)){
        SysDebug(FAIL, QSTR("window system setup FAILED: %d"), status);
        return status;
    }
    SysDebug(PASS, QSTR("window system setup finished"));
    return ISTATUS_SUCCESS;
}


ILIB_API IStatus IntollibCleanup(){
    IStatus status = CleanupMemoryAllocator();
    if(!IS_SUCCESS(status)) return status;
    status = CleanupWindow();
    return status;
}