#ifndef NATIVE_H
#define NATIVE_H

#include <cstdint>

extern void* g_MemoryBase;
const uintptr_t g_ExecutableBase = 0x00400000;

template <typename T>
inline T LoadMemory(int address)
{
    return *(T*)((uintptr_t)g_MemoryBase + address);
}

inline char* LoadPointer(int address)
{
    return *(char**)((uintptr_t)g_MemoryBase + address);
}

inline char* GetAddress(int address)
{
    return (char*)((uintptr_t)g_MemoryBase + address);
}

#endif // NATIVE_H