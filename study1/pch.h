#pragma once
#define WIN32_LEAN_AND_MEAN
#include<windows.h>


struct RCS
{
    ptrdiff_t A;
    ptrdiff_t B;
};

struct Vector2
{
    float x = 0.f, y = 0.f;
};

struct Vector3 : public Vector2
{
    float	z = 0.f;
};
template <typename T>
T ReadMemory(DWORD64 address)
{
    T buffer;
    ReadProcessMemory(GetCurrentProcess(), (LPVOID)address, &buffer, sizeof(T), 0);
    //buffer = *(T*)address;
    return buffer;
}
template <typename T>
void WriteMemory(DWORD64 address, T value)
{
    //*(T*)address = value;
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, &value, sizeof(T), 0);
}

uintptr_t GetClientModule()
{
    return (uintptr_t)GetModuleHandle("client.dll");
}
