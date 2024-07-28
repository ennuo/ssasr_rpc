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
    presence.details = "In Menus";
    Discord_UpdatePresence(&presence);
}

std::string GetExtraRaceDetails(RacerInfo* racer, int race_state)
{
    if (race_state < STATE_Racing)
        return "Waiting to start";

    if (race_state == STATE_Racing)
    {
        return fmt::format("Lap {:d} of {:d}, {}", 
            GetCurrentDisplayLap(), racer->NumLaps - 1, MakePositionDisplay(racer->CurrentPosition));
    }

    return "Finished " + MakePositionDisplay(racer->CurrentPosition);
}

void UpdateRichPresence_Racing(unsigned long start_time)
{
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = start_time;

    int race_state = GetRaceManagerState();
    bool in_gp = InTournament();

    bool racing = race_state == STATE_Racing;
    bool finished = race_state > STATE_Racing;

    MissionInfo* mission = GetMissionInfo();

    int game_type = GetGameType();
    int details_type = kPresenceDetails_None;
    
    // Mission GPs don't have game type set to mission
    // just makes stuff easier to handle to force set it.
    if (mission != nullptr)
        game_type = kGameType_Mission;
    // If we're given a mission game type and the mission is null for whatever reason,
    // just don't update the state so we can at least prevent a crash.
    else if (game_type == kGameType_Mission)
        return;

    const char* game_type_name = GetGameTypeDisplayName();
    std::string details = GetTrackDisplayName();
    RacerInfo* racer = GetRacerInfo();
    std::string state = game_type_name;

    // This shouldn't happen, but going to check anyway just in case
    if (racer != nullptr)
    {
        switch (game_type)
        {
            case kGameType_TimeTrial:
            {
                details_type = kPresenceDetails_TimeTrial;
                break;
            }

            case kGameType_Mission:
            {
                presence.largeImageText = GetTrackDisplayName();
                details = mission->Name;
                state = mission->Type;
                details_type = mission->Presence;

                // Always show your rank in status after a mission is completed.
                if (!in_gp && finished)
                    details_type = kPresenceDetails_Rank;
                
                break;
            }

            case kGameType_Race:
            case kGameType_NetworkRace:
            {
                if (in_gp)
                {
                    details_type = kPresenceDetails_GrandPrix;
                    presence.largeImageText = GetTrackDisplayName();

                    int cup = GetCupNumber();
                    details = "MISTAKES WERE MADE CUP";
                    if (cup >= 0 && cup < g_NumTournaments)
                        details = g_Tournaments[cup];
                }
                else details_type = kPresenceDetails_Race;

                break;
            }
        }

        switch (details_type)
        {
            case kPresenceDetails_Race:
            {
                state = fmt::format("{} - {}", state, GetExtraRaceDetails(racer, race_state));
                break;
            }
            case kPresenceDetails_GrandPrix:
            {
                int race_index = GetCurrentTournamentStageIndex() + 1;
                state = fmt::format("Race {:d} - {}", race_index, GetExtraRaceDetails(racer, race_state));
                break;
            }
            case kPresenceDetails_TimeTrial:
            {
                presence.largeImageText = GetTrackDisplayName();

                details = fmt::format("Time Trial - PB: {}", GetLapTimeString(GetPersonalBestLapTime()));
                state = fmt::format("Session Best: {}", GetLapTimeString(GetSessionBestLapTime()));
                break;
            }
            case kPresenceDetails_Elimination:
            {
                state = fmt::format("{} - Remaining: {:d}", state, GetNumActiveRacers() - GetNumEliminated());
                break;
            }
            case kPresenceDetails_Boss:
            {
                state = fmt::format("{} - Boss Health: {:d}%", state, GetBossHealth());
                break;
            }
            case kPresenceDetails_Rank:
            {
                const char* rank = "None";
                if (racer->MissionRank >= 0 && racer->MissionRank < g_NumRanks)
                    rank = g_Ranks[racer->MissionRank];
                state = fmt::format("{} - Rank: {}", state, rank);
                break;
            }
        }
    }

    presence.state = state.c_str();
    presence.details = details.c_str();

    char* racer_id = GetRacerId();
    if (racer_id != nullptr)
    {
        // Fairly sure I have to convert it to lowercase for Discord?
        // I haven't actually checked honestly.
        std::string racer_id_lower = GetRacerId();
        std::transform(racer_id_lower.begin(), racer_id_lower.end(), racer_id_lower.begin(), 
            [](unsigned char c) { return std::tolower(c); });
        presence.smallImageKey = racer_id_lower.c_str();
    }

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