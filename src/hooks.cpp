#include <MinHook.h>

#include <time.h>

#include "discord.h"
#include "native.h"
#include "ssr.h"

// Just setting it to true so it triggers the startup status
bool g_IsRacing = true;

// Bunch of state variables so we're not constantly updating
// the rich presence when we don't have to.
int g_LastLapCheck = -1;
int g_LastRacePosition = -1;
int g_LastRank = -1;
unsigned int g_LastBestTime;
unsigned long g_RaceStartTime;

// Function calls from the game
GetBestLap_t GetBestLapFromLicense = nullptr;

void(__thiscall *Mission_UpdateMissionLogic)(void*);
void __fastcall OnUpdateMissionLogic(void* self)
{
    Mission_UpdateMissionLogic(self);
    if (!g_IsRacing) return;

    // Only check for rank updates if we're both racing and in a mission.
    MissionInfo* mission = GetMissionInfo();
    RacerInfo* racer = GetRacerInfo();
    if (mission == nullptr || racer == nullptr) return;

    // Sneaking in lap check since it doesn't seem to be called otherwise
    // in mission race/grand prix modes, might just be an oversight on my part though.
    int lap = GetCurrentDisplayLap();
    if (g_LastRank != racer->MissionRank || g_LastLapCheck != lap)
    {
        g_LastLapCheck = lap;
        g_LastRank = racer->MissionRank;
        UpdateRichPresence_Racing(g_RaceStartTime);
    }
}

void(__thiscall *RaceHandler_UpdateCurrentPositions)(void*);
void __fastcall OnUpdateCurrentPositions(void* self)
{
    RaceHandler_UpdateCurrentPositions(self);
    if (!g_IsRacing) return;

    int pos = GetCurrentRacePosition();
    if (g_LastRacePosition != pos)
    {
        g_LastRacePosition = pos;
        UpdateRichPresence_Racing(g_RaceStartTime);
    }
}

void(__thiscall *Racer_OnLapComplete)(void*);
void __fastcall OnLapComplete(void* self)
{
    Racer_OnLapComplete(self);
    if (!g_IsRacing) return;

    // The function is called OnLapComplete, but it seems to be
    // triggered every frame, so we'll just check if our lap check matches
    // Also gonna sneak in the race position check
    // Actually is it only triggered when we're in the race finish segment?
    int game_type = GetGameType();
    if (game_type == kGameType_TimeTrial)
    {
        unsigned int time = GetBestLap();
        if (time != g_LastBestTime)
        {
            g_LastBestTime = time;
            UpdateRichPresence_Racing(g_RaceStartTime);
        }
    }
    else
    {
        int lap = GetCurrentDisplayLap();
        if (g_LastLapCheck != lap)
        {
            g_LastLapCheck = lap;
            UpdateRichPresence_Racing(g_RaceStartTime);
        }
    }
}

void(__cdecl *RaceHandler_Cleanup)(bool);
void __cdecl OnRaceHandlerCleanup(bool b)
{
    RaceHandler_Cleanup(b);
    
    // This gets called a few times during the loading screen,
    // don't want to reset the state timer or update the presence
    // if we don't have to.
    if (!g_IsRacing) return;

    UpdateRichPresence_NotRacing();

    // Reset all our state variables back to their defaults
    g_LastLapCheck = -1;
    g_LastRacePosition = -1;
    g_LastBestTime = -1;
    g_LastRank = -1;
    g_IsRacing = false;
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

void AttachGameFunctions()
{
    GetBestLapFromLicense = (GetBestLap_t)((uintptr_t)g_MemoryBase + 0xBA1C0);
}

void InitHooks()
{
    MH_Initialize();

    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCC930), (void*)&OnRaceHandlerCleanup, (void**)&RaceHandler_Cleanup);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCCE70), (void*)&OnSetIsRacing, (void**)&RaceHandler_SetIsRacing);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xED320), (void*)&OnLapComplete, (void**)&Racer_OnLapComplete);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCD090), (void*)&OnUpdateCurrentPositions, (void**)&RaceHandler_UpdateCurrentPositions);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCB900), (void*)&OnUpdateMissionLogic, (void**)&Mission_UpdateMissionLogic);
    
    MH_EnableHook(MH_ALL_HOOKS);

    AttachGameFunctions();
}

void CloseHooks()
{
    MH_Uninitialize();
}