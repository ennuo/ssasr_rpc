#include "ssr.h"
#include "jenkins.h"

#include <map>
#include <string>

std::map<int, const char*> g_TrackNameMap =
{
    { sumohash("seasidehill_easy"), "Whale Lagoon" },
    { sumohash("seasidehill_medium"), "Ocean Ruin" },
    { sumohash("seasidehill_hard"), "Lost Palace" },
    { sumohash("samba_easy"), "Sunshine Tour" },
    { sumohash("billyhatcher_easy"), "Icicle Valley" },
    
};

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

char* GetRacerSetup()
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
                return *(char**)(racer_data + 0x1f0);
        }

        racer_data += 0x2110;
    }

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

const char* GetGameTypeDisplayName()
{
    int type = GetGameType();
    switch (type)
    {
        // Game seems to idle in the Race state if no other
        case 1: return "Single Race";

        case 2: return "Time Trial";
        case 3: return "Mission";
        case 4: return "Car Viewer";
        case 5: return "Network Race";
        case 6: return "TSO Viewer";
        
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