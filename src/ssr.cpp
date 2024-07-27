#include "ssr.h"

PyramidBlock* GetCurrentMission()
{
    const int CURRENT_MISSION_ADDRESS = 0x912BE0;
    return (PyramidBlock*)LoadPointer(CURRENT_MISSION_ADDRESS);
}

char* GetRaceMission()
{
    const int RACE_MISSION_ADDRESS = 0x911B18;
    return GetAddress(RACE_MISSION_ADDRESS);
}

MissionInfo* GetMissionInfo()
{
    PyramidBlock* block = GetCurrentMission();
    if (block == nullptr) return nullptr;

    auto iter = g_MissionInfoMap.find(block->MissionHash);
    if (iter != g_MissionInfoMap.end())
        return &g_MissionInfoMap[block->MissionHash];

    return nullptr;
}

int GetNumEliminated()
{
    const int NUM_ELIMINATED_ADDRESS = 0x8F7A60;
    return (int)LoadMemory<char>(NUM_ELIMINATED_ADDRESS);
}

int GetBossHealth()
{
    char* mission = GetRaceMission();
    if (mission == nullptr) return 0;

    int score_type = Mission_lpGetScoreType(mission);
    if (score_type == 0x34)
    {
        void* boss_plugin = Mission_lpGetPlugin(mission, 0x1e);
        if (boss_plugin != 0)
        {
            float initial_health = *(float*)((uintptr_t)boss_plugin + 0x42c);
            float health = *(float*)((uintptr_t)boss_plugin + 0x428);

            if (initial_health == 0.0f) return 0;
            
            return (int)((health / initial_health) * 100.0f);
        }
    }

    return 0;
}

char* GetCurrentTournament()
{
    const int TOURNAMENT_ADDRESS = 0x9124F0;
    return LoadPointer(TOURNAMENT_ADDRESS);
}

int GetCurrentTournamentStageIndex()
{
    const int STAGE_INDEX_ADDRESS = 0x9124F4;
    return LoadMemory<int>(STAGE_INDEX_ADDRESS);
}

int GetCupNumber()
{
    const int CUP_NUMBER_ADDRESS = 0x9124E0;
    return LoadMemory<int>(CUP_NUMBER_ADDRESS);
}

bool InTournament()
{
    return GetCurrentTournament() != nullptr;
}

char* GetRaceManager()
{
    const int RACE_MANAGER_ADDRESS = 0x829FC0;
    return LoadPointer(RACE_MANAGER_ADDRESS);
}

int GetNumActiveRacers()
{
    char* manager = GetRaceManager();
    if (manager != nullptr)
        return *(int*)(manager + 0x1100);
    return 0;
}

ExcelData* GetTrackParams()
{
    const int TRACK_PARAMS_ADDRESS = 0x8E8688;
    return (ExcelData*)LoadPointer(TRACK_PARAMS_ADDRESS);
}

char* GetTrackSetup()
{
    char* manager = GetRaceManager();
    if (manager == nullptr) return nullptr;
    char* circuit_data = *(char**)(manager + 0x10e4);
    if (circuit_data == nullptr) return nullptr;
    return *(char**)(circuit_data + 0x1f0);
}

RacerInfo* GetRacerInfo()
{
    const int RACER_INFOS_ADDRESS = 0x8F7A68;
    RacerInfo* racers = (RacerInfo*)LoadPointer(RACER_INFOS_ADDRESS);
    if (racers == nullptr) return nullptr;

    char* my_racer = GetRacer();
    if (my_racer == nullptr) return nullptr;

    int num_racers = GetNumActiveRacers();
    for (int i = 0; i < num_racers; ++i)
    {
        RacerInfo* info = &racers[i];
        if (info->MyRacer == my_racer)
            return info;
    }

    return nullptr;
}

char* GetRacer()
{
    char* manager = GetRaceManager();
    if (manager == nullptr) return nullptr;

    int num_racers = *(int*)(manager + 0x1100);
    if (num_racers == 0) return nullptr;

    char* racer_data = *(char**)(manager + 0x10e8);
    for (int i = 0; i < num_racers; ++i)
    {
        // Check if the racer is a player
        char* control = *(char**)(racer_data + 0x9f0);
        if (control != nullptr)
        {
            int type = *(int*)(control + 0x18);
            // Type 1 is player, 2 is AI, 3 is network player?
            if (type == 1)
                return racer_data;
        }

        racer_data += 0x2110;
    }

    return nullptr;
}

char* GetRacerSetup()
{
    char* racer = GetRacer();
    if (racer != nullptr)
        return *(char**)(racer + 0x1f0);
    return nullptr;
}

char* GetTrackGroupName()
{
    char* track_setup = GetTrackSetup();
    if (track_setup == nullptr) return nullptr;
    return track_setup + 0x168;
}

char* GetTrackId()
{
    char* track_setup = GetTrackSetup();
    if (track_setup == nullptr) return nullptr;
    return track_setup + 0x1ec;
}

char* GetRacerId()
{
    char* racer_setup = GetRacerSetup();
    if (racer_setup == nullptr) return nullptr;
    return racer_setup + 0x170;
}

char* GetRacerDisplayName()
{
    char* racer_setup = GetRacerSetup();
    if (racer_setup == nullptr) return nullptr;
    return racer_setup + 0x190;
}

int GetGameType()
{
    const int GAME_TYPE_ADDRESS = 0x8F7A58;
    return LoadMemory<int>(GAME_TYPE_ADDRESS);
}

unsigned int GetGameLogicRate()
{
    const int GAME_LOGIC_RATE_ADDRESS = 0x82A020;
    return LoadMemory<unsigned int>(GAME_LOGIC_RATE_ADDRESS);
}

unsigned int GetBestLap()
{
    int hash = GetTrackNameHash();
    unsigned int saved_time = GetBestLapFromLicense(hash, 0).Time;
    unsigned int local_time = saved_time;

    RacerInfo* info = GetRacerInfo();
    if (info != nullptr)
        local_time = info->BestLap;
    
    if (local_time == 0 || local_time == -1) return saved_time;

    // I assume generic comparison works for this
    return local_time < saved_time ? local_time : saved_time;
}

bool IsRaceReadyToStart()
{
    const int READY_TO_START_ADDRESS = 0x8F7A50;
    return LoadMemory<bool>(READY_TO_START_ADDRESS);
}

int GetRacerNameHash()
{
    char* racer_setup = GetRacerSetup();
    if (racer_setup == nullptr) return -1;
    return *(int*)(racer_setup + 0x160);
}

int GetTrackNameHash()
{
    char* track_setup = GetTrackSetup();
    if (track_setup == nullptr) return -1;
    return *(int*)(track_setup + 0xc8);
}

int GetCurrentRacePosition()
{
    RacerInfo* info = GetRacerInfo();
    if (info == nullptr) return -1;
    return info->CurrentPosition;
}

int GetCurrentDisplayLap()
{
    int game_type = GetGameType();
    RacerInfo* info = GetRacerInfo();
    if (info == nullptr) return 1;

    int laps;

    int num_laps = info->NumLaps;
    int laps_completed = info->NumLapsCompleted;
    int current_lap = info->CurrentLap;

    switch (game_type)
    {
        case kGameType_TimeTrial:
        {
            laps = 99;
            if (laps_completed < 99)
                laps = laps_completed + 1;
            break;
        }

        case kGameType_Race:
        case kGameType_NetworkRace:
        case kGameType_Mission:
        {
            laps = current_lap;
            if (laps == 0) laps = 1;
            else if (num_laps <= laps)
                laps = num_laps - 1;
            
            break;
        }

        default:
        {
            laps = 1;
            break;
        }
    }

    return laps;
}

const char* GetGameTypeDisplayName()
{
    int type = GetGameType();
    switch (type)
    {
        // Game seems to idle in the Race state if no other
        case kGameType_Race: return "Race";
        
        case kGameType_TimeTrial: return "Time Trial";
        case kGameType_Mission: return "Mission";
        case kGameType_CarViewer: return "Car Viewer";
        case kGameType_NetworkRace: return "Network Race";
        case kGameType_TsoViewer: return "TSO Viewer";
        
        default: return "Invalid";
    }
}

const char* GetTrackDisplayName()
{
    int hash = GetTrackNameHash();

    // Names are only stored in the text packs, which I don't really feel
    // like loading, so we'll just get the names from the hash.
    auto iter = g_TrackNameMap.find(hash);
    if (iter != g_TrackNameMap.end())
        return g_TrackNameMap[hash];

    return "Unknown";
}