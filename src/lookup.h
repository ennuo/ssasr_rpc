#ifndef LOOKUP_H
#define LOOKUP_H

#include <map>

struct MissionInfo {
    const char* Name;
    const char* Type;
};

extern std::map<int, MissionInfo> g_MissionInfoMap;
extern std::map<int, const char*> g_TrackGroupMap;
extern std::map<int, const char*> g_TrackNameMap;

#endif // LOOKUP_H