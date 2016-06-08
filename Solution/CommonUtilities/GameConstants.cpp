#include "stdafx.h"

#include "Defines.h"
#include "GameConstants.h"

bool GC::GenerateLightData = false; 
bool GC::GenerateCubeMap = false;
int GC::SHNodeSize = 4;

bool GC::EnableCheapAmbient = false;
bool GC::EnableVSync = true;
bool GC::UseLowModels = true;

int GC::Difficulty = 10;

float GC::DamageMultiplier[3];
float GC::SpawnIntervalMultiplier[3];

volatile bool GC::PlayerAlive = true;

GC::eMultiplayerMode GC::MultiplayerMode = GC::eMultiplayerMode::HOST;

bool GC::ShouldRenderGUI = true;

float GC::PlayerHeightWithLegs = 0.7f;

float GC::PlayerRadius = 0.7f;

#ifdef DEBUG
bool GC::OptionsUseViberations = false;
#else
bool GC::OptionsUseViberations = true;
#endif
#ifdef SHADOWS
bool GC::OptionsUseShadows = true;
#else
bool GC::OptionsUseShadows = false;
#endif
bool GC::OptionsEnableOffline = false;


bool GC::EnableCheat = false;

int GC::TotalLevels = 0;

int GC::TotalNightmareLevels = 0;

bool GC::NightmareMode = false;
bool GC::HasWonGame = false;
bool GC::HasBeenInVictoryScreen = false;
bool GC::HasBeenInVictoryScreenNightmare = false;

int GC::Gold = 0;

int GC::CurrentActivePlayers = 0;

bool GC::FirstTimeScoreSubmit = false;

int GC::DebugRenderTexture = 0;

float GC::ShadowBufferSize = 512.f;