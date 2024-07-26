#include <windows.h>
#include <discord_rpc.h>

#include <iostream>
#include <stdio.h>
#include <time.h>

#include "ssr.h"

const char* g_ApplicationID = "1266132218418299004";
const char* g_SteamID = "34190";
extern void InitHooks();

// Just setting it to true so it triggers the startup status
bool g_IsRacing = true;

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
    if (!g_IsRacing) return;
    g_IsRacing = false;

    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = time(nullptr);
    presence.largeImageKey = "default";
    Discord_UpdatePresence(&presence);
}

void UpdateRichPresence_Racing()
{
    if (g_IsRacing) return;
    g_IsRacing = true;

    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));
    presence.startTimestamp = time(nullptr);
    presence.details = GetTrackDisplayName();
    //presence.state = GetTrackGroupName();
    presence.state = GetGameTypeDisplayName();
    presence.largeImageKey = "default";

    char racerImageKey[256];
    strncpy(racerImageKey, GetRacerId(), 255);
    racerImageKey[255] = '\0';
    int len = strlen(racerImageKey);
    for (int i = 0; i < len; ++i)
        racerImageKey[i] = tolower(racerImageKey[i]);

    presence.smallImageKey = racerImageKey;
    //presence.smallImageText = GetRacerDisplayName();
    
    Discord_UpdatePresence(&presence);
}

void RpcMain()
{
    InitDiscord();
    InitHooks();
}