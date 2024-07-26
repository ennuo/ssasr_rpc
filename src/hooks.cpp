#include <MinHook.h>

#include "discord.h"
#include "native.h"

void(__cdecl *RaceHandler_Cleanup)(bool);
void __cdecl OnRaceHandlerCleanup(bool b)
{
    UpdateRichPresence_NotRacing();
    RaceHandler_Cleanup(b);
}

void(__cdecl *RaceHandler_SetIsRacing)(bool);
void __cdecl OnSetIsRacing(bool is_racing)
{
    if (is_racing)
        UpdateRichPresence_Racing();
    RaceHandler_SetIsRacing(is_racing);
}

void InitHooks()
{
    MH_Initialize();
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCC930), (void*)&OnRaceHandlerCleanup, (void**)&RaceHandler_Cleanup);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCCE70), (void*)&OnSetIsRacing, (void**)&RaceHandler_SetIsRacing);
    MH_EnableHook(MH_ALL_HOOKS);
}