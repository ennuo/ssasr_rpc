#include <MinHook.h>

#include <time.h>

#include "discord.h"
#include "native.h"
#include "ssr.h"

// Just setting it to true so it triggers the startup status
bool g_IsRacing = true;

// Bunch of state variables so we're not constantly updating
// the rich presence when we don't have to.
int g_Lap = -1;
int g_RacePosition = -1;
int g_MissionRank = -1;
int g_NumEliminated = -1;
int g_BossHealth = 0;
unsigned int g_BestLapTime;
unsigned long g_RaceStartTime;

// Function calls from the game
GetBestLap_t GetBestLapFromLicense = nullptr;
Mission_lpGetScoreType_t Mission_lpGetScoreType;
Mission_lpGetPlugin_t Mission_lpGetPlugin;

void(__thiscall *Mission_UpdateMissionLogic)(void*);
void __fastcall OnUpdateMissionLogic(void* self)
{
    Mission_UpdateMissionLogic(self);
    if (!g_IsRacing) return;

    // Only check for rank updates if we're both racing and in a mission.
    MissionInfo* mission = GetMissionInfo();
    RacerInfo* racer = GetRacerInfo();
    if (mission == nullptr || racer == nullptr) return;

    int eliminees = GetNumEliminated();
    int health = GetBossHealth();
    if (g_MissionRank != racer->MissionRank || eliminees != g_NumEliminated || health != g_BossHealth)
    {
        g_NumEliminated = eliminees;
        g_BossHealth = health;
        g_MissionRank = racer->MissionRank;
        UpdateRichPresence_Racing(g_RaceStartTime);
    }
}

void(__thiscall *RaceHandler_UpdateCurrentPositions)(void*);
void __fastcall OnUpdateCurrentPositions(void* self)
{
    RaceHandler_UpdateCurrentPositions(self);
    if (!g_IsRacing) return;

    int pos = GetCurrentRacePosition();
    if (g_RacePosition != pos)
    {
        g_RacePosition = pos;
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
        if (time != g_BestLapTime)
        {
            g_BestLapTime = time;
            UpdateRichPresence_Racing(g_RaceStartTime);
        }
    }
    else
    {
        int lap = GetCurrentDisplayLap();
        if (g_Lap != lap)
        {
            g_Lap = lap;
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
    g_Lap = -1;
    g_RacePosition = -1;
    g_BestLapTime = -1;
    g_MissionRank = -1;
    g_NumEliminated = -1;
    g_BossHealth = 0;
    g_IsRacing = false;
}

void(__cdecl *RaceHandler_SetIsRacing)(bool);
void __cdecl OnSetIsRacing(bool is_racing)
{
    if (is_racing && !g_IsRacing)
    {
        g_RaceStartTime = time(nullptr);
        g_IsRacing = true;
        g_Lap = GetCurrentDisplayLap();
        g_RacePosition = GetCurrentRacePosition();
        
        UpdateRichPresence_Racing(g_RaceStartTime);
    }

    RaceHandler_SetIsRacing(is_racing);
}

void AttachGameFunctions()
{
    GetBestLapFromLicense = (GetBestLap_t)((uintptr_t)g_MemoryBase + 0xBA1C0);
    Mission_lpGetScoreType = (Mission_lpGetScoreType_t)((uintptr_t)g_MemoryBase + 0xC8C90);
    Mission_lpGetPlugin = (Mission_lpGetPlugin_t)((uintptr_t)g_MemoryBase + 0xC8BA0);
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