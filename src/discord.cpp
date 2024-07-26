#include <windows.h>
#include <discord_rpc.h>
#include <fmt/core.h>

#include <time.h>

#include <algorithm>
#include <cctype>
#include <string>

#include "ssr.h"

const char* g_ApplicationID = "1266132218418299004";
const char* g_SteamID = "34190";
extern void InitHooks();

void InitDiscord()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    Discord_Initialize(g_ApplicationID, &handlers, 1, g_SteamID);
}

void CloseDiscord()
{
    Discord_Shutdown();
}

void UpdateRichPresence_NotRacing()
{
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = time(nullptr);
    presence.largeImageKey = "default";
    Discord_UpdatePresence(&presence);
}

std::string MakePositionDisplay(int position)
{
    position = position + 1;
    std::string suffix;
    switch (position)
    {
        case 1: suffix = "ˢᵗ"; break;
        case 2: suffix = "ⁿᵈ"; break;
        case 3: suffix = "ʳᵈ"; break;
        default: suffix = "ᵗʰ"; break;
    }

    return std::to_string(position) + suffix;
}

void UpdateRichPresence_Racing(unsigned long start_time)
{
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = start_time;

    int game_type = GetGameType();
    const char* game_type_name = GetGameTypeDisplayName();
    std::string details = GetTrackDisplayName();
    RacerInfo* racer = GetRacerInfo();
    std::string state;

    // This shouldn't happen, but going to check anyway just in case
    if (racer == nullptr) state = game_type_name;
    else if (game_type == kGameType_TimeTrial)
    {
        int time = racer->BestLap;
        state = fmt::format("{} - PB: {}", game_type_name, "--:--:--");
    }
    else if (game_type == kGameType_Race || game_type == kGameType_NetworkRace)
    {
        state = fmt::format("{} (Lap {:d} of {:d}, {})", game_type_name, GetCurrentDisplayLap(), racer->NumLaps - 1, MakePositionDisplay(racer->CurrentPosition));
    }

    presence.state = state.c_str();
    presence.details = details.c_str();
    
    std::string racer_id = GetRacerId();
    std::transform(racer_id.begin(), racer_id.end(), racer_id.begin(), 
        [](unsigned char c) { return std::tolower(c); });
    presence.smallImageKey = racer_id.c_str();

    // Set track preview image as large image
    const char* track_id = GetTrackId();
    if (track_id == nullptr) track_id = "default";
    presence.largeImageKey = track_id;
    
    Discord_UpdatePresence(&presence);
}

void RpcMain()
{
    InitDiscord();
    InitHooks();
}