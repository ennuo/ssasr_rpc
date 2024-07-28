#include <MinHook.h>

#include <time.h>

#include "discord.h"
#include "native.h"
#include "ssr.h"

// Bunch of state variables so we're not constantly updating
// the rich presence when we don't have to.
int g_Lap = -1;
int g_RacePosition = -1;
int g_MissionRank = -1;
int g_NumEliminated = -1;
int g_BossHealth = 0;
unsigned int g_BestLapTime;
unsigned long g_RaceStartTime;
int g_RaceState;

// Function calls from the game
GetBestLap_t GetBestLapFromLicense = nullptr;
Mission_lpGetScoreType_t Mission_lpGetScoreType;
Mission_lpGetPlugin_t Mission_lpGetPlugin;

void(__thiscall *Mission_UpdateMissionLogic)(void*);
void __fastcall OnUpdateMissionLogic(void* self)
{
    Mission_UpdateMissionLogic(self);

    // Only check for rank updates if we're both racing and in a mission.
    MissionInfo* mission = GetMissionInfo();
    RacerInfo* racer = GetRacerInfo();
    if (mission == nullptr || racer == nullptr) return;

    bool changes = false;
    switch (mission->Presence)
    {
        case kPresenceDetails_Elimination:
        {
            int eliminees = GetNumEliminated();
            if (eliminees != g_NumEliminated)
            {
                g_NumEliminated = eliminees;
                changes = true;
            }
            break;
        }
        case kPresenceDetails_Boss:
        {
            int health = GetBossHealth();
            if (health != g_BossHealth)
            {
                g_BossHealth = health;
                changes = true;
            }
            break;
        }
        default:
        {
            if (g_MissionRank != racer->MissionRank)
            {
                g_MissionRank = racer->MissionRank;
                changes = true;
            }
            break;
        }
    }

    if (changes)
        UpdateRichPresence_Racing(g_RaceStartTime);
}

void(__thiscall *RaceHandler_UpdateCurrentPositions)(void*);
void __fastcall OnUpdateCurrentPositions(void* self)
{
    RaceHandler_UpdateCurrentPositions(self);
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

    // The function is called OnLapComplete, but it seems to be
    // triggered every frame, so we'll just check if our lap check matches
    // Also gonna sneak in the race position check
    // Actually is it only triggered when we're in the race finish segment?
    int game_type = GetGameType();
    if (game_type == kGameType_TimeTrial)
    {
        unsigned int time = GetSessionBestLapTime();
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

void(__thiscall *RaceHandler_Update)(void*);
void __fastcall OnUpdateRaceState(void* self)
{
    RaceHandler_Update(self);

    int race_state = GetRaceManagerState();
    if (race_state != g_RaceState)
    {
        g_RaceState = race_state;
        bool gp = InTournament();

        // We've gone back to the menu
        if (race_state == STATE_Idle && !gp)
        {
            UpdateRichPresence_NotRacing();
            return;
        }

        // Make sure the time doesn't reset between GP races
        if (race_state == STATE_WaitingToStart && (!gp || GetCurrentTournamentStageIndex() == 0))
        {
            g_RaceStartTime = time(nullptr);
        }

        if (race_state >= STATE_WaitingToStart && race_state <= STATE_RaceOver)
            UpdateRichPresence_Racing(g_RaceStartTime);
    }
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

    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xED320), (void*)&OnLapComplete, (void**)&Racer_OnLapComplete);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCD090), (void*)&OnUpdateCurrentPositions, (void**)&RaceHandler_UpdateCurrentPositions);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCB900), (void*)&OnUpdateMissionLogic, (void**)&Mission_UpdateMissionLogic);
    MH_CreateHook((void*)((uintptr_t)g_MemoryBase + 0xCBB80), (void*)&OnUpdateRaceState, (void**)&RaceHandler_Update);
    
    MH_EnableHook(MH_ALL_HOOKS);

    AttachGameFunctions();
}

void CloseHooks()
{
    MH_Uninitialize();
}