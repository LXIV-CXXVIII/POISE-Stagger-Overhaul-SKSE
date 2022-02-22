#include "C:/dev/simpleini-master/SimpleIni.h"

#include "Loki_PoiseMod.h"
#include "Loki_PluginTools.h"

void Loki_PoiseMod::ReadPoiseIni(const wchar_t* a_filename, std::unordered_map<RE::BGSKeyword*, float*> a_map) {

    CSimpleIniA ini;
    ini.SetUnicode();
    auto filename = a_filename;
    SI_Error rc = ini.LoadFile(filename);
    //ini.setmul

    std::list<CSimpleIniA::Entry> keyList = {};
    std::list<CSimpleIniA::Entry> valueList = {};
    ini.GetAllKeys("FORM_IDS", keyList);
    for (auto idx : keyList) { // for every entry in keyList do
        logger::info("For every entry in keylist");
        RE::FormID formID = NULL;
        RE::BSFixedString espName = NULL;
        std::string item = idx.pItem; // get key string
        std::string colon = ":";      // prep for comp
        for (int i = 0; i < 64; i++) {  // iterate through string
            logger::info("interate through key");
            if (item[i] == colon[0]) {   // if ini string == : then do
                logger::info("found what we need");
                item[i] = 0;             // split esp and formID
                espName = (RE::BSFixedString)(const char*)item[0]; // found the esp name
                auto cFormID = std::stoi((const char*)item[i + 1]); // convert string form ID to Int
                formID = (RE::FormID)cFormID;  // found the formID 
                auto dataHandle = RE::TESDataHandler::GetSingleton();
                auto kywd = dataHandle->LookupForm<RE::BGSKeyword>(formID, espName); // get the keyword ptr
                //ini.GetAllValues("FORM_IDS", idx.pItem, valueList);
                //a_map.insert_or_assign(kywd ? kywd : nullptr, (float*)valueList.begin()->pItem);  // assign keyword and ini settings to map
                bool* f = {};
                std::string value = (std::string)ini.GetValue("FORM_IDS", idx.pItem, "null", f); // get value string
                float* mapStruct = {}; // allocate float pointer
                std::string comma = ","; // prep for comp
                for (int i = 0; i < 64; i++) { // iterate through string
                    logger::info("iterate through value");
                    if (value[i] == comma[0]) { // if value string == , then do
                        logger::info("found what we need");
                        value[i] = 0;           // split values
                        mapStruct[0] = std::stof((std::string)(const char*)value[0]); // convert string to float and assign to float pointer
                        mapStruct[1] = std::stof((std::string)(const char*)value[i + 1]); // same as above but for the 2nd value
                        logger::info("break out of value assignment");
                        break;  // break out of string comp
                    }
                }
                a_map.insert_or_assign(kywd ? kywd : nullptr, mapStruct); // assign keyword and ini settings to map
                logger::info("assign struct to map");
                //a_map.insert_or_assign(kywd ? kywd : nullptr, (float)ini.GetAllValues("FORM_IDS", idx.pItem, valueList));
                //a_map.insert_or_assign(kywd ? kywd : nullptr, (float)ini.GetDoubleValue("FORM_IDS", idx.pItem, -1.00f)); // assign keyword and ini setting to map
                break;
            }
        }
    }

}

Loki_PoiseMod::Loki_PoiseMod() {

    //Loki_PoiseMod::ReadPoiseIni(L"Data/SKSE/Plugins/loki_POISE/loki_POISE_RaceHealthSettings.ini", this->healthKywdMap);
        //Loki_PoiseMod::ReadPoiseIni(L"Data/SKSE/Plugins/loki_POISE/loki_POISE_RaceDamageSettings.ini", this->damageKywdMap);

    CSimpleIniA ini;
    ini.SetUnicode();
    auto filename = L"Data/SKSE/Plugins/loki_POISE.ini";
    SI_Error rc = ini.LoadFile(filename);

    this->ConsoleInfoDump = ini.GetBoolValue("DEBUG", "bConsoleInfoDump", false);

    this->PoiseSystemEnabled = ini.GetBoolValue("MAIN", "bPoiseSystem", false);
    this->PlayerRagdollReplacer = ini.GetBoolValue("MAIN", "bPlayerRagdollReplacer", false);
    this->NPCRagdollReplacer = ini.GetBoolValue("MAIN", "bNPCRagdollReplacer", false);
    this->PoiseRegenEnabled = ini.GetBoolValue("MAIN", "bPoiseRegen", false);
    this->TrueHUDBars = ini.GetBoolValue("MAIN", "bTrueHUDBars", false);

    this->CreaturePoiseHealthMult = ini.GetDoubleValue("ENEMY", "fCreaturePoiseHealthMult", -1.00f);
    this->DragonPoiseHealthMult = ini.GetDoubleValue("ENEMY", "fDragonPoiseHealthMult", -1.00f);
    this->DwarvenPoiseHealthMult = ini.GetDoubleValue("ENEMY", "fDwarvenPoiseHealthMult", -1.00f);

    this->CreaturePoiseDamageMult = ini.GetDoubleValue("ENEMY", "fCreaturePoiseDamageMult", -1.00f);
    this->DragonPoiseDamageMult = ini.GetDoubleValue("ENEMY", "fDragonPoiseDamageMult", -1.00f);
    this->DwarvenPoiseDamageMult = ini.GetDoubleValue("ENEMY", "fDwarvenPoiseDamageMult", -1.00f);

    this->PowerAttackMult = ini.GetDoubleValue("WEAPON", "fPowerAttackMult", -1.00f);
    this->BlockedMult = ini.GetDoubleValue("WEAPON", "fBlockedMult", -1.00f);
    this->BashMult = ini.GetDoubleValue("WEAPON", "fBashMult", -1.00f);
    this->HyperArmourMult = ini.GetDoubleValue("WEAPON", "fHyperArmourMult", -1.00f);
    this->BowMult = ini.GetDoubleValue("WEAPON", "fBowMult", -1.00f);
    this->CrossbowMult = ini.GetDoubleValue("WEAPON", "fCrossbowMult", -1.00f);
    this->Hand2Hand = ini.GetDoubleValue("WEAPON", "fHand2HandMult", -1.00f);
    this->OneHandAxe = ini.GetDoubleValue("WEAPON", "fOneHandAxeMult", -1.00f);
    this->OneHandDagger = ini.GetDoubleValue("WEAPON", "fOneHandDaggerMult", -1.00f);
    this->OneHandMace = ini.GetDoubleValue("WEAPON", "fOneHandMaceMult", -1.00f);
    this->OneHandSword = ini.GetDoubleValue("WEAPON", "fOneHandSwordMult", -1.00f);
    this->TwoHandAxe = ini.GetDoubleValue("WEAPON", "fTwoHandAxeMult", -1.00f);
    this->TwoHandSword = ini.GetDoubleValue("WEAPON", "fTwoHandSwordMult", -1.00f);

    this->RapierMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fRapierMult", -1.00f);
    this->PikeMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fPikeMult", -1.00f);
    this->SpearMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fSpearMult", -1.00f);
    this->HalberdMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fHalberdMult", -1.00f);
    this->QtrStaffMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fQtrStaffMult", -1.00f);
    this->CaestusMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fCaestusMult", -1.00f);
    this->ClawMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fClawMult", -1.00f);
    this->WhipMult = ini.GetDoubleValue("ANIMATED_ARMOURY", "fWhipMult", -1.00f);

    static RE::TESDataHandler* dataHandle = NULL;
    if (!dataHandle) {
        dataHandle = RE::TESDataHandler::GetSingleton();
        if (dataHandle) {
            poiseDelaySpell = dataHandle->LookupForm<RE::SpellItem>(0xD62, "loki_POISE.esp");
            poiseDelayEffect = dataHandle->LookupForm<RE::EffectSetting>(0xD63, "loki_POISE.esp");
            PoiseDmgNerf = dataHandle->LookupForm<RE::BGSKeyword>(0x433C, "loki_POISE.esp");
            PoiseDmgBuff = dataHandle->LookupForm<RE::BGSKeyword>(0x433B, "loki_POISE.esp");
            PoiseHPNerf = dataHandle->LookupForm<RE::BGSKeyword>(0x433A, "loki_POISE.esp");
            PoiseHPBuff = dataHandle->LookupForm<RE::BGSKeyword>(0x4339, "loki_POISE.esp");

            kCreature = dataHandle->LookupForm<RE::BGSKeyword>(0x13795, "Skyrim.esm");
            kDragon = dataHandle->LookupForm<RE::BGSKeyword>(0x35d59, "Skyrim.esm");
            kGiant = dataHandle->LookupForm<RE::BGSKeyword>(0x10E984, "Skyrim.esm");
            kGhost = dataHandle->LookupForm<RE::BGSKeyword>(0xd205e, "Skyrim.esm");
            kDwarven = dataHandle->LookupForm<RE::BGSKeyword>(0x1397a, "Skyrim.esm");
            kTroll = dataHandle->LookupForm<RE::BGSKeyword>(0xf5d16, "Skyrim.esm");
            WeapMaterialSilver = dataHandle->LookupForm<RE::BGSKeyword>(0x10aa1a, "Skyrim.esm");
        }
    }

}

Loki_PoiseMod* Loki_PoiseMod::GetSingleton() {
    static Loki_PoiseMod* singleton = new Loki_PoiseMod();
    return singleton;
}

void Loki_PoiseMod::InstallStaggerHook() {
    REL::Relocation<std::uintptr_t> StaggerHook{ REL::ID(37673/*628c20*/) };

    auto& trampoline = SKSE::GetTrampoline();
    _ProcessHitEvent = trampoline.write_call<5>(StaggerHook.address() + 0x3C0, ProcessHitEvent);

    logger::info("ProcessHitEvent hook injected");
}

void Loki_PoiseMod::InstallWaterHook() {
    REL::Relocation<std::uintptr_t> ActorUpdate{ REL::ID(36357) };
    // last ditch effort
    auto& trampoline = SKSE::GetTrampoline();
    _GetSubmergedLevel = trampoline.write_call<5>(ActorUpdate.address() + 0x6D3, GetSubmergedLevel);
}

void Loki_PoiseMod::InstallIsActorKnockdownHook() {
    REL::Relocation<std::uintptr_t> isActorKnockdown{ REL::ID(38858) };

    auto& trampoline = SKSE::GetTrampoline();
    _IsActorKnockdown = trampoline.write_call<5>(isActorKnockdown.address() + 0x7E, IsActorKnockdown);

    logger::info("isActorKnockdown hook injected");
}

void Loki_PoiseMod::InstallVFuncHooks() {
    //REL::Relocation<std::uintptr_t> ActorVtbl{ RE::Offset::Actor::Vtbl };
    //_HandleHealthDamage_Actor = ActorVtbl.write_vfunc(0x104, HandleHealthDamage_Actor);

    REL::Relocation<std::uintptr_t> CharacterVtbl{ REL::ID(261397) };  // 165DA40
    _HandleHealthDamage_Character = CharacterVtbl.write_vfunc(0x104, HandleHealthDamage_Character);

    REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::Offset::PlayerCharacter::Vtbl };
    _HandleHealthDamage_PlayerCharacter = PlayerCharacterVtbl.write_vfunc(0x104, HandleHealthDamage_PlayerCharacter);
}

float Loki_PoiseMod::CalculatePoiseDamage(RE::HitData& a_hitData, RE::Actor* a_actor) {

    // this whole function is BAD and DIRTY but i cant think of any other way at the moment

    auto ptr = Loki_PoiseMod::GetSingleton();

    bool bolk;
    bool atak;
    a_actor->GetGraphVariableBool("IsBlocking", bolk);
    a_actor->GetGraphVariableBool("IsAttacking", atak);
    auto aggressor = a_hitData.aggressor.get();

    /*
    for (auto idx : ptr->damageKywdMap) {
        if (idx.first && a_actor->HasKeyword(idx.first)) {
            auto creatureResult = aggressor->GetWeight();
            if (bolk) {
                return (creatureResult * idx.second[1]) * ptr->BlockedMult;
            }
            if (atak) {
                return (creatureResult * idx.second[1]) * ptr->HyperArmourMult;
            }
            return (creatureResult * idx.second[1]);
        }
    }
    */
    if ((aggressor->HasKeyword(ptr->kCreature) && !aggressor->HasKeyword(ptr->kDragon) && !aggressor->HasKeyword(ptr->kGiant))) {
        if (bolk) {
            return (aggressor->GetWeight() * ptr->CreaturePoiseDamageMult) * ptr->BlockedMult;
        }
        if (atak) {
            return (aggressor->GetWeight() * ptr->CreaturePoiseDamageMult) * ptr->HyperArmourMult;
        }
        return (aggressor->GetWeight() * ptr->CreaturePoiseDamageMult);
    } else if (aggressor->HasKeyword(ptr->kDragon)) {
        if (bolk) {
            return (aggressor->GetWeight() * ptr->DragonPoiseDamageMult) * ptr->BlockedMult;
        }
        if (atak) {
            return (aggressor->GetWeight() * ptr->DragonPoiseDamageMult) * ptr->HyperArmourMult;
        }
        return (aggressor->GetWeight() * ptr->DragonPoiseDamageMult);
    } else if (aggressor->HasKeyword(ptr->kDwarven)) {
        if (bolk) {
            return (aggressor->GetWeight() * ptr->DwarvenPoiseDamageMult) * ptr->BlockedMult;
        }
        if (atak) {
            return (aggressor->GetWeight() * ptr->DwarvenPoiseDamageMult) * ptr->HyperArmourMult;
        }
        return (aggressor->GetWeight() * ptr->DwarvenPoiseDamageMult);
    }

    auto weap = a_hitData.weapon;
    float a_result = 0.00f;
    if (!weap) {
        auto attacker = a_hitData.aggressor.get();
        if (!attacker) {
            return 8.00f;
        }
        auto attackerWeap = attacker->GetAttackingWeapon();
        if (!attackerWeap) {
            return 8.00f;
        }
        return attackerWeap->GetWeight();
    }
    a_result = weap->weight;

    switch (weap->weaponData.animationType.get()) {
    case RE::WEAPON_TYPE::kBow:
        a_result *= ptr->BowMult;
        break;

    case RE::WEAPON_TYPE::kCrossbow:
        a_result *= ptr->CrossbowMult;
        break;

    case RE::WEAPON_TYPE::kHandToHandMelee:
        if (weap->HasKeyword(0x19AAB3)) {
            a_result *= ptr->CaestusMult;
            break;
        }
        if (weap->HasKeyword(0x19AAB4)) {
            a_result *= ptr->ClawMult;
            break;
        }
        a_result *= ptr->Hand2Hand;
        break;

    case RE::WEAPON_TYPE::kOneHandAxe:
        a_result *= ptr->OneHandAxe;
        break;

    case RE::WEAPON_TYPE::kOneHandDagger:
        a_result *= ptr->OneHandDagger;
        break;

    case RE::WEAPON_TYPE::kOneHandMace:
        a_result *= ptr->OneHandMace;
        break;

    case RE::WEAPON_TYPE::kOneHandSword:
        if (weap->HasKeyword(0x801)) {
            a_result *= ptr->RapierMult;
            break;
        }
        a_result *= ptr->OneHandSword;
        break;

    case RE::WEAPON_TYPE::kTwoHandAxe:
        if (weap->HasKeyword(0xE4580)) {
            a_result *= ptr->HalberdMult;
            break;
        }
        if (weap->HasKeyword(0xE4581)) {
            a_result *= ptr->QtrStaffMult;
            break;
        }
        a_result *= ptr->TwoHandAxe;
        break;

    case RE::WEAPON_TYPE::kTwoHandSword:
        if (weap->HasKeyword(0xE457E)) {
            a_result *= ptr->PikeMult;
            break;
        }
        if (weap->HasKeyword(0xE457F)) {
            a_result *= ptr->SpearMult;
            break;
        }
        a_result *= ptr->TwoHandSword;
        break;

    }

    RE::BSFixedString buffKeyword = "PoiseDmgBuff";
    RE::BSFixedString nerfKeyword = "PoiseDmgNerf";

    auto hasBuff = Loki_PluginTools::ActorHasEffectWithKeyword(aggressor.get(), buffKeyword);
    if (hasBuff) {
        logger::info("damage buff keyword detected");
        auto buffPercent = hasBuff->effectItem.magnitude / 100.00f; // convert to percentage
        auto resultingBuff = (a_result * buffPercent);
        a_result += resultingBuff; // aggressor has buff that makes them do more poise damage
    }
    auto hasNerf = Loki_PluginTools::ActorHasEffectWithKeyword(aggressor.get(), nerfKeyword);
    if (hasNerf) {
        logger::info("damage nerf keyword detected");
        auto nerfPercent = hasNerf->effectItem.magnitude / 100.00f;
        auto resultingNerf = (a_result * nerfPercent);
        a_result -= resultingNerf; // aggressor has buff that makes them do less poise damage
    }

    if (a_hitData.flags == RE::HitData::Flag::kPowerAttack) {
        a_result *= ptr->PowerAttackMult;
    }

    if (a_hitData.flags == RE::HitData::Flag::kBash) {
        a_result *= ptr->BashMult;
    }

    bool blk, atk;
    a_actor->GetGraphVariableBool("IsBlocking", blk);
    a_actor->GetGraphVariableBool("IsAttacking", atk);
    if (blk) {
        a_result *= ptr->BlockedMult;
    }
    if (atk) {
        a_result *= ptr->HyperArmourMult;
    }
    if ((a_actor->HasKeyword(ptr->kGhost) && !weap->HasKeyword(ptr->WeapMaterialSilver))) {
        a_result = 0.00f;
    }

    return a_result;

}
float Loki_PoiseMod::CalculateMaxPoise(RE::Actor* a_actor) {

    auto ptr = Loki_PoiseMod::GetSingleton();

    float a_result = (a_actor->equippedWeight + (a_actor->GetBaseActorValue(RE::ActorValue::kHeavyArmor) * 0.20f));

    /*
    for (auto idx : ptr->healthKywdMap) {
        if (idx.first && a_actor->HasKeyword(idx.first)) {
            return a_result * idx.second[0];
        }
    }
    */


    if ((a_actor->HasKeyword(ptr->kCreature) && !a_actor->HasKeyword(ptr->kDragon))) {
        a_result = a_actor->GetWeight() * ptr->CreaturePoiseHealthMult;
    } else if (a_actor->HasKeyword(ptr->kDragon)) {
        a_result = a_actor->GetWeight() * ptr->DragonPoiseHealthMult;
    } else if (a_actor->HasKeyword(ptr->kDwarven)) {
        a_result = a_actor->GetWeight() * ptr->DwarvenPoiseHealthMult;
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

bool Loki_PoiseMod::IsActorKnockdown(RE::Character* a_this, std::int64_t a_unk) {

    auto ptr = Loki_PoiseMod::GetSingleton();

    auto avHealth = a_this->GetActorValue(RE::ActorValue::kHealth);
    if (a_this->IsOnMount() || avHealth <= 0.00f) {
        return _IsActorKnockdown(a_this, a_unk);
    }
    static RE::BSFixedString str = NULL;
    if (a_this->IsPlayerRef()) {
        if (ptr->PlayerRagdollReplacer) {
            float knockdownDirection = 0.00f;
            a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);
            if (knockdownDirection > 0.25f && knockdownDirection < 0.75f) {
                str = ptr->poiseLargestFwd;
            } else {
                str = ptr->poiseLargestBwd;
            }
            a_this->NotifyAnimationGraph(str);
            return false;
        }
    } else {
        if (ptr->NPCRagdollReplacer) {
            float knockdownDirection = 0.00f;
            a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);
            if (knockdownDirection > 0.25f && knockdownDirection < 0.75f) {
                str = ptr->poiseLargestFwd;
            } else {
                str = ptr->poiseLargestBwd;
            }
            a_this->NotifyAnimationGraph(str);
            return false;
        }
    }
    return _IsActorKnockdown(a_this, a_unk);

}

float Loki_PoiseMod::GetSubmergedLevel(RE::Actor* a_actor, float a_zPos, RE::TESObjectCELL* a_cell) {

    auto ptr = Loki_PoiseMod::GetSingleton();

    auto avHealth = a_actor->GetActorValue(RE::ActorValue::kHealth);
    if (avHealth <= 0.05f || !ptr->PoiseRegenEnabled) { return _GetSubmergedLevel(a_actor, a_zPos, a_cell); }

    if (!a_actor->HasMagicEffect(ptr->poiseDelayEffect)) {
        auto a_result = (int)Loki_PoiseMod::CalculateMaxPoise(a_actor);
        a_actor->pad0EC = a_result;
        if (a_actor->pad0EC > 100000) { a_actor->pad0EC = 0.00f; }
    }

    return _GetSubmergedLevel(a_actor, a_zPos, a_cell);

}

void Loki_PoiseMod::HandleHealthDamage_Actor(RE::Actor* a_actor, RE::Actor* a_attacker, float a_damage) {

    auto ptr = Loki_PoiseMod::GetSingleton();
    auto hitData = a_attacker->currentProcess->middleHigh->lastHitData;
    if (hitData->attackData->data.attackSpell) {
        auto avHealth = a_actor->GetActorValue(RE::ActorValue::kHealth);
        auto avParalysis = a_actor->GetActorValue(RE::ActorValue::kParalysis);
        if (avHealth <= 0.05f || a_actor->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _HandleHealthDamage_Actor(a_actor, a_attacker, a_damage); }

        if (hitData->healthDamage <= 0.00f) hitData->healthDamage = 0.00f;
        a_actor->pad0EC -= (int)hitData->healthDamage; // there was some bullshit with integer underflow
        if (a_actor->pad0EC > 100000) a_actor->pad0EC = 0.00f; // this fixed it...

        auto hitPos = a_attacker->GetPosition();
        auto heading = a_actor->GetHeadingAngle(hitPos, false);
        auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
        if (a_actor->GetHandle() == a_attacker->GetHandle()) { stagDir = 0.0f; } // 0 when self-hit

        auto a = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        a->Cast(ptr->poiseDelaySpell, false, a_actor, 1.0f, false, 0.0f, 0);

        float maxPoise = Loki_PoiseMod::CalculateMaxPoise(a_actor);
        auto prcnt25 = maxPoise * 0.25f;
        auto prcnt35 = maxPoise * 0.35f;
        auto prcnt50 = maxPoise * 0.50f;

        bool isBlk = false;
        static RE::BSFixedString str = NULL;
        a_actor->GetGraphVariableBool(ptr->isBlocking, isBlk);
        if ((float)a_actor->pad0EC <= 0.00f) {
            a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            a_actor->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
            if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_actor->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                a_actor->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargestFwd;
                    } else {
                        str = ptr->poiseLargestBwd;
                    }
                    a_actor->NotifyAnimationGraph(str);         // if those, play tier 4
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    a_actor->NotifyAnimationGraph(str);  // if not those, play tier 2
                }
            }
        } else if ((float)a_actor->pad0EC < prcnt25 || (float)a_actor->pad0EC < 2.00f) {
            a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_actor->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    isBlk ? hitData->pushBack = 7.5f : a_actor->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_actor->pad0EC < prcnt35 || (float)a_actor->pad0EC < 5.00f) {
            a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_actor->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseSmallFwd;
                    } else {
                        str = ptr->poiseSmallBwd;
                    }
                    isBlk ? hitData->pushBack = 5.0f : a_actor->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_actor->pad0EC < prcnt50 || (float)a_actor->pad0EC < 10.00f) {
            a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseSmallFwd;
                } else {
                    str = ptr->poiseSmallBwd;
                }
                isBlk ? hitData->pushBack = 2.50f : a_actor->NotifyAnimationGraph(str);
            }
        };
    };

    logger::info("HealthDamage_Actor executed");
    return _HandleHealthDamage_Actor(a_actor, a_attacker, a_damage);

}

void Loki_PoiseMod::HandleHealthDamage_Character(RE::Character* a_char, RE::Actor* a_attacker, float a_damage) {

    auto ptr = Loki_PoiseMod::GetSingleton();
    auto hitData = a_attacker->currentProcess->middleHigh->lastHitData;
    if (hitData->attackData->data.attackSpell) {
        auto avHealth = a_char->GetActorValue(RE::ActorValue::kHealth);
        auto avParalysis = a_char->GetActorValue(RE::ActorValue::kParalysis);
        if (avHealth <= 0.05f || a_char->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _HandleHealthDamage_Character(a_char, a_attacker, a_damage); }

        if (hitData->healthDamage <= 0.00f) hitData->healthDamage = 0.00f;
        a_char->pad0EC -= (int)hitData->healthDamage; // there was some bullshit with integer underflow
        if (a_char->pad0EC > 100000) a_char->pad0EC = 0.00f; // this fixed it...

        auto hitPos = a_attacker->GetPosition();
        auto heading = a_char->GetHeadingAngle(hitPos, false);
        auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
        if (a_char->GetHandle() == a_attacker->GetHandle()) { stagDir = 0.0f; } // 0 when self-hit

        auto a = a_char->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        a->Cast(ptr->poiseDelaySpell, false, a_char, 1.0f, false, 0.0f, 0);

        float maxPoise = Loki_PoiseMod::CalculateMaxPoise(a_char);
        auto prcnt25 = maxPoise * 0.25f;
        auto prcnt35 = maxPoise * 0.35f;
        auto prcnt50 = maxPoise * 0.50f;

        bool isBlk = false;
        static RE::BSFixedString str = NULL;
        a_char->GetGraphVariableBool(ptr->isBlocking, isBlk);
        if ((float)a_char->pad0EC <= 0.00f) {
            a_char->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            a_char->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
            if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_char->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                a_char->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargestFwd;
                    } else {
                        str = ptr->poiseLargestBwd;
                    }
                    a_char->NotifyAnimationGraph(str);         // if those, play tier 4
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    a_char->NotifyAnimationGraph(str);  // if not those, play tier 2
                }
            }
        } else if ((float)a_char->pad0EC < prcnt25 || (float)a_char->pad0EC < 2.00f) {
            a_char->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                a_char->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_char->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    isBlk ? hitData->pushBack = 7.5f : a_char->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_char->pad0EC < prcnt35 || (float)a_char->pad0EC < 5.00f) {
            a_char->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_char->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_char->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseSmallFwd;
                    } else {
                        str = ptr->poiseSmallBwd;
                    }
                    isBlk ? hitData->pushBack = 5.0f : a_char->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_char->pad0EC < prcnt50 || (float)a_char->pad0EC < 10.00f) {
            a_char->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_char->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseSmallFwd;
                } else {
                    str = ptr->poiseSmallBwd;
                }
                isBlk ? hitData->pushBack = 2.50f : a_char->NotifyAnimationGraph(str);
            }
        };
    };

    logger::info("HealthDamage_Character executed");
    return _HandleHealthDamage_Character(a_char, a_attacker, a_damage);

}

void Loki_PoiseMod::HandleHealthDamage_PlayerCharacter(RE::PlayerCharacter* a_playerChar, RE::Actor* a_attacker, float a_damage) {

    auto ptr = Loki_PoiseMod::GetSingleton();
    auto hitData = a_attacker->currentProcess->middleHigh->lastHitData;
    if (hitData->attackData->data.attackSpell) {
        auto avHealth = a_playerChar->GetActorValue(RE::ActorValue::kHealth);
        auto avParalysis = a_playerChar->GetActorValue(RE::ActorValue::kParalysis);
        if (avHealth <= 0.05f || a_playerChar->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _HandleHealthDamage_PlayerCharacter(a_playerChar, a_attacker, a_damage); }

        if (hitData->healthDamage <= 0.00f) hitData->healthDamage = 0.00f;
        a_playerChar->pad0EC -= (int)hitData->healthDamage; // there was some bullshit with integer underflow
        if (a_playerChar->pad0EC > 100000) a_playerChar->pad0EC = 0.00f; // this fixed it...

        auto hitPos = a_attacker->GetPosition();
        auto heading = a_playerChar->GetHeadingAngle(hitPos, false);
        auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
        if (a_playerChar->GetHandle() == a_attacker->GetHandle()) { stagDir = 0.0f; } // 0 when self-hit

        auto a = a_playerChar->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        a->Cast(ptr->poiseDelaySpell, false, a_playerChar, 1.0f, false, 0.0f, 0);

        float maxPoise = Loki_PoiseMod::CalculateMaxPoise(a_playerChar);
        auto prcnt25 = maxPoise * 0.25f;
        auto prcnt35 = maxPoise * 0.35f;
        auto prcnt50 = maxPoise * 0.50f;

        bool isBlk = false;
        static RE::BSFixedString str = NULL;
        a_playerChar->GetGraphVariableBool(ptr->isBlocking, isBlk);
        if ((float)a_playerChar->pad0EC <= 0.00f) {
            a_playerChar->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            a_playerChar->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
            if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargestFwd;
                    } else {
                        str = ptr->poiseLargestBwd;
                    }
                    a_playerChar->NotifyAnimationGraph(str);         // if those, play tier 4
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    a_playerChar->NotifyAnimationGraph(str);  // if not those, play tier 2
                }
            }
        } else if ((float)a_playerChar->pad0EC < prcnt25 || (float)a_playerChar->pad0EC < 2.00f) {
            a_playerChar->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_playerChar->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseMedFwd;
                    } else {
                        str = ptr->poiseMedBwd;
                    }
                    isBlk ? hitData->pushBack = 7.5f : a_playerChar->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_playerChar->pad0EC < prcnt35 || (float)a_playerChar->pad0EC < 5.00f) {
            a_playerChar->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (hitData->flags == RE::HitData::Flag::kExplosion) {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseLargeFwd;
                    } else {
                        str = ptr->poiseLargeBwd;
                    }
                    a_playerChar->NotifyAnimationGraph(str);
                } else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseSmallFwd;
                    } else {
                        str = ptr->poiseSmallBwd;
                    }
                    isBlk ? hitData->pushBack = 5.0f : a_playerChar->NotifyAnimationGraph(str);
                }
            }
        } else if ((float)a_playerChar->pad0EC < prcnt50 || (float)a_playerChar->pad0EC < 10.00f) {
            a_playerChar->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseSmallFwd;
                } else {
                    str = ptr->poiseSmallBwd;
                }
                isBlk ? hitData->pushBack = 2.50f : a_playerChar->NotifyAnimationGraph(str);
            }
        };
    };

    logger::info("HealthDamage_PlayerCharacter executed");
    return _HandleHealthDamage_PlayerCharacter(a_playerChar, a_attacker, a_damage);

}

void Loki_PoiseMod::ProcessHitEvent(RE::Actor* a_actor, RE::HitData& a_hitData) {

    RE::FormID kLurker = 0x14495;

    auto ptr = Loki_PoiseMod::GetSingleton();

    using HitFlag = RE::HitData::Flag;

    auto avHealth = a_actor->GetActorValue(RE::ActorValue::kHealth);
    auto avParalysis = a_actor->GetActorValue(RE::ActorValue::kParalysis);
    if (avHealth <= 0.05f || a_actor->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _ProcessHitEvent(a_actor, a_hitData); }

    float dmg = Loki_PoiseMod::CalculatePoiseDamage(a_hitData, a_actor);

    if (dmg <= 0.00f) dmg = 0.00f;
    a_actor->pad0EC -= (int)dmg;  // there was some bullshit with integer underflow
    if (a_actor->pad0EC > 100000) a_actor->pad0EC = 0.00f;  // this fixed it...
    if (ptr->ConsoleInfoDump) {
        RE::ConsoleLog::GetSingleton()->Print("---");
        RE::ConsoleLog::GetSingleton()->Print("Aggessor's Weight: %f", a_hitData.aggressor.get()->GetWeight());
        RE::ConsoleLog::GetSingleton()->Print("Aggressor's Current Poise Health: %d", a_hitData.aggressor.get()->pad0EC);
        RE::ConsoleLog::GetSingleton()->Print("Aggresssor's Max Poise Health: %f", Loki_PoiseMod::CalculateMaxPoise(a_hitData.aggressor.get().get()));
        RE::ConsoleLog::GetSingleton()->Print("Aggressor's Poise Damage: %f", dmg);
        RE::ConsoleLog::GetSingleton()->Print("-");
        RE::ConsoleLog::GetSingleton()->Print("Victim's Weight: %f", a_actor->GetWeight());
        RE::ConsoleLog::GetSingleton()->Print("Victim's Current Poise Health: %d", a_actor->pad0EC);
        RE::ConsoleLog::GetSingleton()->Print("Victim's Max Poise Health %f", Loki_PoiseMod::CalculateMaxPoise(a_actor));
        RE::ConsoleLog::GetSingleton()->Print("---");
    }

    auto hitPos = a_hitData.aggressor.get()->GetPosition();
    auto heading = a_actor->GetHeadingAngle(hitPos, false);
    auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
    if (a_actor->GetHandle() == a_hitData.aggressor) { stagDir = 0.0f; } // 0 when self-hit

    auto a = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
    a->Cast(ptr->poiseDelaySpell, false, a_actor, 1.0f, false, 0.0f, 0);

    float maxPoise = Loki_PoiseMod::CalculateMaxPoise(a_actor);
    auto prcnt25 = maxPoise * 0.25f;
    auto prcnt35 = maxPoise * 0.35f;
    auto prcnt50 = maxPoise * 0.50f;

    bool isBlk = false;
    static RE::BSFixedString str = NULL;
    a_actor->GetGraphVariableBool(ptr->isBlocking, isBlk);
    if ((float)a_actor->pad0EC <= 0.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        a_actor->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
        //Loki_TrueHUDControl::GetSingleton()->g_trueHUD->FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), false);
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
        } else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {  // check if explosion, dragon, giant attack or dwarven
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargestFwd;
                } else {
                    str = ptr->poiseLargestBwd;
                }
                a_actor->NotifyAnimationGraph(str);         // if those, play tier 4
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseMedFwd;
                } else {
                    str = ptr->poiseMedBwd;
                }
                a_actor->NotifyAnimationGraph(str);  // if not those, play tier 2
            }
        }
    } else if ((float)a_actor->pad0EC < prcnt25 || (float)a_actor->pad0EC < 2.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {  // check if explosion, dragon, giant attack or dwarven
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargeFwd;
                } else {
                    str = ptr->poiseLargeBwd;
                }
                a_actor->NotifyAnimationGraph(str);           // if those, play tier 3
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseMedFwd;
                } else {
                    str = ptr->poiseMedBwd;
                }
                isBlk ? a_hitData.pushBack = 5.00 : a_actor->NotifyAnimationGraph(str); // if block, set pushback, ! play tier 2
            }
        }
    } else if ((float)a_actor->pad0EC < prcnt35 || (float)a_actor->pad0EC < 5.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) {
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargeFwd;
                } else {
                    str = ptr->poiseLargeBwd;
                }
                a_actor->NotifyAnimationGraph(str);  // play tier 3 again
            } else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseSmallFwd;
                } else {
                    str = ptr->poiseSmallBwd;
                }
                isBlk ? a_hitData.pushBack = 3.75 : a_actor->NotifyAnimationGraph(str);
            }
        }
    } else if ((float)a_actor->pad0EC < prcnt50 || (float)a_actor->pad0EC < 10.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) {
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.25f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } else {
            if (stagDir > 0.25f && stagDir < 0.75f) {
                str = ptr->poiseSmallFwd;
            } else {
                str = ptr->poiseSmallBwd;
            }
            isBlk ? a_hitData.pushBack = 2.50f : a_actor->NotifyAnimationGraph(str);
        }
    }

    return _ProcessHitEvent(a_actor, a_hitData);

}