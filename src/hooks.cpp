#include <MinHook.h>

#include <time.h>

#include "discord.h"
#include "native.h"
#include "ssr.h"

// Just setting it to true so it triggers the startup status
bool g_IsRacing = true;
int g_LastLapCheck = -1;
int g_LastRacePosition = -1;
unsigned long g_RaceStartTime;

void(__thiscall *RaceHandler_UpdateCurrentPositions)(void*);
void __fastcall OnUpdateCurrentPositions(void* self)
{
    if (g_IsRacing)
    {
        int pos = GetCurrentRacePosition();
        if (g_LastRacePosition != pos)
        {
            g_LastRacePosition = pos;

            UpdateRichPresence_Racing(g_RaceStartTime);
        }
    }

    RaceHandler_UpdateCurrentPositions(self);
}

void(__thiscall *Racer_OnLapComplete)(void*);
void __fastcall OnLapComplete(void* self)
{
    // The function is called OnLapComplete, but it seems to be
    // triggered every frame, so we'll just check if our lap check matches
    // Also gonna sneak in the race position check
    // Actually is it only triggered when we're in the race finish segment?
    if (g_IsRacing)
    {
        int pos = GetCurrentRacePosition();
        if (g_LastRacePosition != pos)
        {
            g_LastRacePosition = pos;
            UpdateRichPresence_Racing(g_RaceStartTime);
        }
    }

    Racer_OnLapComplete(self);
}

void(__cdecl *RaceHandler_Cleanup)(bool);
void __cdecl OnRaceHandlerCleanup(bool b)
{
    // This gets called a few times during the loading screen,
    // don't want to reset the state timer or update the presence
    // if we don't have to.
    if (g_IsRacing)
    {
        UpdateRichPresence_NotRacing();
        g_LastLapCheck = -1;
        g_LastRacePosition = -1;
        g_IsRacing = false;
    }

    RaceHandler_Cleanup(b);
}

void(__cdecl *RaceHandler_SetIsRacing)(bool);
void __cdecl OnSetIsRacing(bool is_racing)
{
    if (is_racing && !g_IsRacing)
    {
        g_RaceStartTime = time(nullptr);
        g_IsRacing = true;
        g_LastLapCheck = GetCurrentDisplayLap();
        g_LastRacePosition = GetCurrentRacePosition();
        
        UpdateRichPresence_Racing(g_RaceStartTime);
    }

    RaceHandler_SetIsRacing(is_racing);
}

void InitHooks()
{
    MH_Initialize();
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCC930), (void*)&OnRaceHandlerCleanup, (void**)&RaceHandler_Cleanup);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCCE70), (void*)&OnSetIsRacing, (void**)&RaceHandler_SetIsRacing);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xED320), (void*)&OnLapComplete, (void**)&Racer_OnLapComplete);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCD090), (void*)&OnUpdateCurrentPositions, (void**)&RaceHandler_UpdateCurrentPositions);
    MH_EnableHook(MH_ALL_HOOKS);
}