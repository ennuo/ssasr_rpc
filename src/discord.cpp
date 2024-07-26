#include <windows.h>
#include <discord_rpc.h>
#include <fmt/core.h>

#include <iostream>
#include <stdio.h>
#include <time.h>

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

void UpdateRichPresence_Racing(unsigned long start_time)
{
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = start_time;
    presence.details = GetTrackDisplayName();

    RacerInfo* racer = GetRacerInfo();
    std::string state;
    
    // This shouldn't happen, but going to check anyway just in case
    if (racer == nullptr)
    {
        state = GetGameTypeDisplayName();
    }
    else
    {
        state = fmt::format("{} (Lap {:d} of {:d})", GetGameTypeDisplayName(), GetCurrentDisplayLap(), racer->NumLaps - 1);
    }

    presence.state = state.c_str();
    presence.largeImageKey = GetTrackId();

    char racerImageKey[256];
    strncpy(racerImageKey, GetRacerId(), 255);
    racerImageKey[255] = '\0';
    int len = strlen(racerImageKey);
    for (int i = 0; i < len; ++i)
        racerImageKey[i] = tolower(racerImageKey[i]);

    presence.smallImageKey = racerImageKey;
    
    Discord_UpdatePresence(&presence);
}

void RpcMain()
{
    InitDiscord();
    InitHooks();
}