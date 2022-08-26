#include "TrueHUDControl.h"

Loki::TrueHUDControl::TrueHUDControl() {
    CSimpleIniA ini;
    ini.SetUnicode();
    auto filename = L"Data/SKSE/Plugins/loki_POISE.ini";
    SI_Error rc = ini.LoadFile(filename);

    this->TrueHUDBars = ini.GetBoolValue("MAIN", "bTrueHUDBars", false);
}

Loki::TrueHUDControl* Loki::TrueHUDControl::GetSingleton() {
    static Loki::TrueHUDControl* singleton = new Loki::TrueHUDControl();
    return singleton;
}

float Loki::TrueHUDControl::GetMaxSpecial([[maybe_unused]] RE::Actor* a_actor) {

    auto ptr = Loki::PoiseMod::GetSingleton();

    float a_result = (a_actor->equippedWeight + (a_actor->GetBaseActorValue(RE::ActorValue::kHeavyArmor) * 0.20f));

    for (auto idx : ptr->poiseRaceMap) {
        if (a_actor) {
            RE::TESRace* a_actorRace = a_actor->race;
            RE::TESRace* a_mapRace = idx.first;
            if (a_actor && a_actorRace && a_mapRace) {
                if (a_actorRace->formID == a_mapRace->formID) {
                    a_result = idx.second[0];
                    break;
                }
            }
        }
    }

    auto hasBuff = Loki::PluginTools::ActorHasEffectWithKeyword(a_actor, ptr->PoiseHPBuff->formID);
    if (hasBuff) {
        auto buffPercent = hasBuff->effectItem.magnitude / 100.00f; // convert to percentage
        auto resultingBuff = (a_result * buffPercent);
        a_result += resultingBuff;
    }
    auto hasNerf = Loki::PluginTools::ActorHasEffectWithKeyword(a_actor, ptr->PoiseDmgNerf->formID);
    if (hasNerf) {
        auto nerfPercent = hasNerf->effectItem.magnitude / 100.00f;
        auto resultingNerf = (a_result * nerfPercent);
        a_result -= resultingNerf;
    }

    return a_result;

}

float Loki::TrueHUDControl::GetCurrentSpecial([[maybe_unused]] RE::Actor* a_actor) {
    return (float)a_actor->pad0EC;
}