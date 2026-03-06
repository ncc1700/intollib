#include "nanoprintf/nprintfimpl.h"
#include <Windows.h>
#include <intollib.h>


ILIB_API void SysPrintCStr(const char* cstr){
    unsigned long amount = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), 
            cstr, 
            GetStringLength((u8*)cstr), 
            &amount, NULL);
}

ILIB_API void SysPrint(String str){
    char* cstr = QStringToCStringHeap(str);
    unsigned long amount = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), 
            cstr, str.length, 
            &amount, NULL);
    FreeMemory(cstr);
}

ILIB_API void SysDebug(DebugType type, String str, ...){
    switch(type){
        case PASS:
            SysPrint(QSTR("[PASS] "));
            break;
        case FAIL:
            SysPrint(QSTR("[FAIL] "));
            break;
        case INFO:
            SysPrint(QSTR("[INFO] "));
            break;
        case WARNING:
            SysPrint(QSTR("[WARNING] "));
            break;
        default:
            break;
    }
    // very bad
    // TODO: make better
    char buffer[256];
    String trueStr;
    if(str.length >= 127){
        trueStr = SliceString(str, 126);
    } else {
        trueStr = DupeString(str);
    }
    char* cstr = QStringToCStringHeap(trueStr);
    va_list arg;
    va_start(arg, str.buffer);
    ImplVsnprintf(buffer, 256, 
                    cstr, arg);
    va_end(arg);
    SysPrint(QSTR(buffer));
    SysPrint(QSTR("\n"));
    FreeMemory(cstr);
}