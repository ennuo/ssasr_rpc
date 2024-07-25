#include <windows.h>
#include <discord_rpc.h>

const char* g_ApplicationID = "1266132218418299004";
const char* g_SteamID = "34190";

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

DWORD WINAPI RpcMain(LPVOID lpParameter)
{
    InitDiscord();


    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(DiscordRichPresence));

    presence.state = "LUCKY JAMES LERRY IS A FRAUD";

    Discord_UpdatePresence(&presence);

    while (true)
    {
        Sleep(5000);
    }

    CloseDiscord();
    return 0;
}

