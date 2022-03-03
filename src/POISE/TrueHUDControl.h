#pragma once
#include "TrueHUDAPI.h"
#include "C:/dev/simpleini-master/SimpleIni.h"

#include "PoiseMod.h"
#include "Loki_PluginTools.h"

namespace Loki {

    class TrueHUDControl {

    public:
        TrueHUDControl();
        static TrueHUDControl* GetSingleton();
        static float GetMaxSpecial([[maybe_unused]] RE::Actor* a_actor);
        static float GetCurrentSpecial([[maybe_unused]] RE::Actor* a_actor);

        bool TrueHUDBars;
        TRUEHUD_API::IVTrueHUD1* g_trueHUD = NULL;
    };

};