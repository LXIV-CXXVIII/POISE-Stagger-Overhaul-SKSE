#pragma once
#include "TrueHUDAPI.h"
#include "C:/dev/simpleini-master/SimpleIni.h"

#include "Loki_PoiseMod.h"
#include "Loki_PluginTools.h"

class Loki_TrueHUDControl {

public:
    Loki_TrueHUDControl();
    static Loki_TrueHUDControl* GetSingleton();
    static float GetMaxSpecial([[maybe_unused]] RE::Actor* a_actor);
    static float GetCurrentSpecial([[maybe_unused]] RE::Actor* a_actor);

    bool TrueHUDBars;
    TRUEHUD_API::IVTrueHUD1* g_trueHUD = NULL;
};