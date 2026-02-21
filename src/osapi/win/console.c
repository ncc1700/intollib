#include "nanoprintf/nprintfimpl.h"
#include <Windows.h>
#include <intollib.h>



ILIB_API void SysPrint(String str){
    unsigned long amount = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), 
            str.buffer, str.length, 
            &amount, NULL);
}

ILIB_API void SysFormatPrint(String str, ...){
    // very bad
    // TODO: make better
    char buffer[256];
    char cstr[128];

    u64 cindex = 0;
    for(cindex = 0; cindex < str.length; cindex++){
        if(cindex >= 127) break;
        cstr[cindex] = str.buffer[cindex];
    }
    cstr[cindex] = '\0';
    va_list arg;
    va_start(arg, C(str.buffer));
    ImplVsnprintf(buffer, 256, 
                    cstr, arg);
    va_end(arg);
    SysPrint(QSTR(buffer));
}