#include "Loki_TrueHUDControl.h"

Loki_TrueHUDControl::Loki_TrueHUDControl() {
    CSimpleIniA ini;
    ini.SetUnicode();
    auto filename = L"Data/SKSE/Plugins/loki_POISE.ini";
    SI_Error rc = ini.LoadFile(filename);

    this->TrueHUDBars = ini.GetBoolValue("MAIN", "bTrueHUDBars", false);
}

Loki_TrueHUDControl* Loki_TrueHUDControl::GetSingleton() {
    static Loki_TrueHUDControl* singleton = new Loki_TrueHUDControl();
    return singleton;
}

float Loki_TrueHUDControl::GetMaxSpecial([[maybe_unused]] RE::Actor* a_actor) {

    auto ptr = Loki_PoiseMod::GetSingleton();

    float a_result = (a_actor->equippedWeight + (a_actor->GetBaseActorValue(RE::ActorValue::kHeavyArmor) * 0.20f));

    for (auto idx : ptr->poiseRaceMap) {
        if (a_actor && (a_actor->race->formID == idx.first->formID)) {
            a_result = a_actor->GetWeight() * idx.second[0];
        }
    }

    RE::BSFixedString buffKeyword = "MaxPoiseBuff";
    RE::BSFixedString nerfKeyword = "MaxPoiseNerf";

    auto hasBuff = Loki_PluginTools::ActorHasEffectWithKeyword(a_actor, buffKeyword);
    if (hasBuff) {
        logger::info("health buff keyword detected");
        auto buffPercent = hasBuff->effectItem.magnitude / 100.00f; // convert to percentage
        auto resultingBuff = (a_result * buffPercent);
        a_result += resultingBuff;
    }
    auto hasNerf = Loki_PluginTools::ActorHasEffectWithKeyword(a_actor, nerfKeyword);
    if (hasNerf) {
        logger::info("health nerf keyword detected");
        auto nerfPercent = hasNerf->effectItem.magnitude / 100.00f;
        auto resultingNerf = (a_result * nerfPercent);
        a_result -= resultingNerf;
    }

    return a_result;

}

float Loki_TrueHUDControl::GetCurrentSpecial([[maybe_unused]] RE::Actor* a_actor) {
    return (float)a_actor->pad0EC;
}