#pragma once

#include <string.h>
#include <stdint.h>
#include <stdio.h>

class Goodstr
{
    char* handle;
    uint64_t limit;
public:
    Goodstr() : limit(256) {handle = (char*)malloc(256);}
    ~Goodstr() {free(handle);}
    template<typename... Args> void format(const char* format, Args... args)
    {
        memset(handle, 0, 256);
        snprintf(handle, 256, format, args...);
    }
    void print() {printf("%s", handle);}
    const char* cstr() {return (const char*)handle;}
    int length() {return strnlen(cstr(), limit);}
};