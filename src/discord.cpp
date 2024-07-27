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
extern void CloseHooks();

const int g_NumRankStrings = 7;
const char* g_RankStrings[] = { "AAA", "AA", "A", "B", "C", "D", "E" };

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

std::string GetLapTimeString(unsigned int time)
{
    if (time == -1 || time == 0)
        return "--:--.---";

    unsigned int logic_rate = GetGameLogicRate();

    // There's probably some nicer way to represent this, but I just stole
    // it straight from the Ghidra disassembly.

    unsigned int fractional = ((time % (logic_rate << 0xc)) * 1000) / logic_rate >> 0xc;
    unsigned int seconds = time / logic_rate >> 0xc;
    unsigned int minutes = seconds / 0x3c;
    if (minutes / 0x3c != 0)
    {
        seconds = 0x3b;
        minutes = 0x3b;
        fractional = 999;
    }

    return fmt::format("{:02d}:{:02d}.{:03d}", minutes % 0x3c, seconds % 0x3c, fractional);
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

    MissionInfo* mission = GetMissionInfo();

    int game_type = GetGameType();

    // Some missions use race/gp game types, and I don't want to deal with them at this very moment
    // will handle it at some point though, probably.
    if (mission != nullptr)
        game_type = kGameType_Mission;

    const char* game_type_name = GetGameTypeDisplayName();
    std::string details = GetTrackDisplayName();
    RacerInfo* racer = GetRacerInfo();
    std::string state = game_type_name;

    bool add_race_details = false;

    // This shouldn't happen, but going to check anyway just in case
    if (racer != nullptr)
    {
        switch (game_type)
        {
            case kGameType_TimeTrial:
            {
                state = fmt::format("{} - PB: {}", game_type_name, GetLapTimeString(GetBestLap()));
                break;
            }

            case kGameType_Mission:
            {
                presence.largeImageText = GetTrackDisplayName();
                details = mission->Name;
                state = mission->Type;

                if (strcmp(mission->Type, "Race") == 0 || strcmp(mission->Type, "Grand Prix") == 0)
                {
                    add_race_details = true;
                }
                else if (strcmp(mission->Type, "Knockout") == 0)
                {
                    state = fmt::format("{} - Remaining: {:d}", state, GetNumActiveRacers());
                }
                else
                {
                    const char* rank = "None";
                    if (racer->MissionRank >= 0 && racer->MissionRank < g_NumRankStrings)
                        rank = g_RankStrings[racer->MissionRank];
                    state = fmt::format("{} - Rank: {}", state, rank);
                }

                break;
            }

            case kGameType_Race:
            case kGameType_NetworkRace:
            {
                add_race_details = true;
                break;
            }
        }
    }

    if (add_race_details)
    {
        state = fmt::format("{} (Lap {:d} of {:d}, {})", state, GetCurrentDisplayLap(), racer->NumLaps - 1, MakePositionDisplay(racer->CurrentPosition));
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

void RpcClose()
{
    CloseDiscord();
    CloseHooks();
}