#ifndef SSR_H
#define SSR_H

#include "native.h"

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

struct RacerInfo {
    float MissionScore;
    int MissionRank;
    int CurrentCheckpoint;
    bool CheckpointsPassed[16];
    int CheckpointTimes[16];
    int TimeAtLastCheckpoint;
    int BestLap;
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

int GetNumActiveRacers();
int GetCurrentDisplayLap();
int GetGameType();
const char* GetGameTypeDisplayName();
bool IsRaceReadyToStart();

#endif // SSR_H