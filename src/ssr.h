#ifndef SSR_H
#define SSR_H

#include "native.h"
#include "lookup.h"

enum GameType
{
    kGameType_None,
    kGameType_Race,
    kGameType_TimeTrial,
    kGameType_Mission,
    kGameType_CarViewer,
    kGameType_NetworkRace,
    kGameType_TsoViewer
};

struct PyramidBlock {
    const char* DisplayName;
    const char* MissionName;
    unsigned int MissionHash;
    unsigned int StarsToUnlock;
    unsigned int Tier;
    unsigned int MissionNumber;
    unsigned int BestScore;
    unsigned int NumStars;
    bool Played;
    bool InitiallyUnlocked;
};

struct RacerInfo {
    float MissionScore;
    int MissionRank;
    int CurrentCheckpoint;
    bool CheckpointsPassed[16];
    int CheckpointTimes[16];
    int TimeAtLastCheckpoint;
    unsigned int BestLap;
    bool LastLapWasNewBestLap;
    int TotalTimeForCompletedLaps;
    int* LapTimes;
    int RaceBackwardsCount;
    int CurrentLap;
    int LastLapTime;
    int NumLapsCompleted;
    int CurrentPosition;
    int NumLaps;
    float InitialTrackDistance;
    void* MyRacer;
};

struct BestTime {
    unsigned int RacerHash;
    unsigned int Time;
    bool UserSet;
};

struct ExcelCellData {
    int Name;
    int Type;
    union
    {
        unsigned int ui32;
        int i32;
        float f32;
        char* str;
    };
};

struct Column {
    int NumValues;
    int HashName;
    char Name[64];
    ExcelCellData Data[1];
};

struct Worksheet {
    int HashName;
    char Name[64];
    int NumColumns;
    Column Columns[1];
};

struct ExcelData {
    char Header[4];
    int NumWorksheets;
    Worksheet* Worksheets[1];
};

PyramidBlock* GetCurrentMission();
char* GetRaceMission();
MissionInfo* GetMissionInfo();
int GetNumEliminated();
int GetBossHealth();

char* GetCurrentTournament();
bool InTournament();
int GetCurrentTournamentStageIndex();
int GetCupNumber();

ExcelData* GetTrackParams();
char* GetTrackSetup();
char* GetTrackId();
int GetTrackNameHash();
const char* GetTrackDisplayName();
char* GetTrackGroupName();

char* GetRacer();
RacerInfo* GetRacerInfo();
char* GetRacerSetup();
char* GetRacerId();
int GetRacerNameHash();
char* GetRacerDisplayName();

unsigned int GetGameLogicRate();
int GetNumActiveRacers();
int GetCurrentDisplayLap();
int GetCurrentRacePosition();
int GetGameType();
const char* GetGameTypeDisplayName();
bool IsRaceReadyToStart();
bool IsRacing();

typedef BestTime&(__stdcall *GetBestLap_t)(int track, int index);
typedef int(__fastcall *Mission_lpGetScoreType_t)(void* mission);
typedef void*(__thiscall *Mission_lpGetPlugin_t)(void* mission, int plugin);

extern GetBestLap_t GetBestLapFromLicense;
extern Mission_lpGetScoreType_t Mission_lpGetScoreType;
extern Mission_lpGetPlugin_t Mission_lpGetPlugin;

unsigned int GetPersonalBestLapTime();
unsigned int GetSessionBestLapTime();

#endif // SSR_H