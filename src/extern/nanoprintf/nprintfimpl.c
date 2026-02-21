#define NANOPRINTF_IMPLEMENTATION
#include <nanoprintf/nanoprintf.h>
#include <nanoprintf/nprintfimpl.h>


int ImplVsnprintf(char *const buffer, const size_t bufferCount, const char *const format, va_list argList){
    return npf_vsnprintf(buffer, bufferCount, format, argList);
}



