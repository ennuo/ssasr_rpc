#ifndef SSR_H
#define SSR_H

#include "native.h"
#include "lookup.h"

#include <string>

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

enum StateID
{
    STATE_Undefined,
    STATE_Control_AIOffTrack,
    STATE_Control_TrappedRecovery,
    STATE_Control_AIRacing,
    STATE_Control_ChaseHQ,
    STATE_Control_AIRacing_DebugDriver,
    STATE_Control_AIRacing_Pacifist,
    STATE_Control_AIRacing_Finish,
    STATE_Control_AIRacing_Fadeout,
    STATE_Control_AIWrongWay,
    STATE_Control_AIRacing_Autopilot,
    STATE_Control_AIRacing_AutopilotOnRails,
    STATE_Control_AIRacing_Erratic,
    STATE_Control_AIRacing_ScreenFlip,
    STATE_Control_AIRacing_Rainbow,
    STATE_Control_Respot,
    STATE_Control_Restart,
    STATE_Control_Human,
    STATE_Control_Human_Erratic,
    STATE_Control_Human_MonkeyBall,
    STATE_Control_Idle,
    STATE_Control_Network,
    STATE_Control_Ghost,
    STATE_Control_StartingGridHuman,
    STATE_Control_StartingGridAI,
    STATE_Control_Traffic,
    STATE_Control_Podium,
    STATE_Control_Mission_Eliminated,
    STATE_Control_Parked,
    STATE_Control_Mission_Pickup,
    STATE_Control_Mission_Avoid,
    STATE_Control_Mission_Shoot,
    STATE_Control_AIRacing_Coasting,
    STATE_Control_AIRacing_DisconnectedController,
    STATE_Control_AIRacing_RollingStart,
    STATE_Control_AIRacing_RollingStart_Human,
    STATE_Control_AIRacing_RollingStart_Human_KeepDistance,
    STATE_Control_AIRacing_RollingStart_AI,
    STATE_Control_AIRacing_RollingStart_AI_KeepDistance,
    STATE_Control_AIStopped,
    STATE_Control_MissionOver_Invisible,
    STATE_Tournament_Idle,
    STATE_Tournament_Loading,
    STATE_Tournament_Starting,
    STATE_Tournament_StartingMission,
    STATE_Tournament_MissionComplete,
    STATE_Tournament_Complete,
    STATE_Tournament_PlayingMission,
    STATE_Tournament_EndSequence,
    STATE_Idle,
    STATE_Loading,
    STATE_SetupObjectManager,
    STATE_SetupRace,
    STATE_RestartRace,
    STATE_RestartRaceWithReload,
    STATE_WaitingToStart,
    STATE_WaitingToStartNoIntro,
    STATE_TrackIntro,
    STATE_CharacterIntro,
    STATE_Countdown,
    STATE_Racing,
    STATE_WaitingForFinishers,
    STATE_RaceOver,
    STATE_Results,
    STATE_Podium,
    STATE_RetryExit,
    STATE_AutoSave,
    STATE_SegaMiles,
    STATE_MissionModeComplete,
    STATE_MissionInstructions,
    STATE_Race_Results,
    STATE_GrandPrixPodium,
    STATE_TimeTrial_Results,
    STATE_TimeTrial_Leaderboards,
    STATE_TimeTrial_UploadGhost,
    STATE_Mission_Results,
    STATE_Mission_League,
    STATE_Mission_GPUnlock,
    STATE_Mission_GroupCupWin,
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

int GetRaceManagerState();
int GetTournamentManagerState();

std::string GetLapTimeString(unsigned int time);
std::string MakePositionDisplay(int position);

#endif // SSR_H