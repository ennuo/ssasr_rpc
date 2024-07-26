#ifndef SSR_H
#define SSR_H

#include "native.h"

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
int GetNumActiveRacers();
char* GetRacerSetup();
char* GetTrackSetup();
char* GetTrackGroupName();
char* GetTrackId();
char* GetRacerId();
char* GetRacerDisplayName();
int GetGameType();
int GetRacerNameHash();
int GetTrackNameHash();
const char* GetTrackDisplayName();
const char* GetGameTypeDisplayName();
bool IsRaceReadyToStart();


#endif // SSR_H