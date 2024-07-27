#ifndef LOOKUP_H
#define LOOKUP_H

#include <map>

enum PresenceDetailsType
{
    kPresenceDetails_None,
    kPresenceDetails_Race,
    kPresenceDetails_GrandPrix,
    kPresenceDetails_Elimination,
    kPresenceDetails_Boss,
    kPresenceDetails_TimeTrial,
    kPresenceDetails_Rank
};

struct MissionInfo {
    const char* Name;
    const char* Type;
    int Presence;
};

extern std::map<int, MissionInfo> g_MissionInfoMap;
extern std::map<int, const char*> g_TrackGroupMap;
extern std::map<int, const char*> g_TrackNameMap;

#endif // LOOKUP_H