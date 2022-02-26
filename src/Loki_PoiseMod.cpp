#include "Loki_PoiseMod.h"

void Loki_PoiseMod::ReadPoiseTOML() {

    constexpr auto path = L"Data/SKSE/Plugins/loki_POISE";
    constexpr auto ext = L".toml";
    constexpr auto basecfg = L"Data/SKSE/Plugins/loki_POISE/loki_POISE_RaceSettings.toml";

    auto dataHandle = RE::TESDataHandler::GetSingleton();

    const auto readToml = [&](std::filesystem::path path) {
        logger::info("Reading {}...", path.string());
        try {

            const auto tbl = toml::parse_file(path.c_str());
            auto& arr = *tbl.get_as<toml::array>("race");
            for (auto&& elem : arr) {
                auto& raceTable = *elem.as_table();

                auto formID = raceTable["FormID"].value<RE::FormID>();
                logger::info("FormID -> {}", *formID);
                auto plugin = raceTable["Plugin"].value<std::string_view>();
                logger::info("plugin -> {}", *plugin);
                auto race = dataHandle->LookupForm<RE::TESRace>(*formID, *plugin);

                auto poiseMults = raceTable["PoiseMults"].as_array();
                if (poiseMults) {
                    std::vector<float> muls = {};
                    for (auto& MulValue : *poiseMults) {
                        logger::info("multiplier -> {}", *MulValue.value<float>());
                        muls.push_back(*MulValue.value<float>());
                    }
                    poiseRaceMap.insert_or_assign(race ? race : nullptr, muls);
                }
            }

        } catch (const toml::parse_error& e) {
            std::ostringstream ss;
            ss << "Error parsing file \'" << *e.source().path << "\':\n"
                << '\t' << e.description() << '\n'
                << "\t\t(" << e.source().begin << ')';
            logger::error(ss.str());
        } catch (const std::exception& e) {
            logger::error("{}", e.what());
        } catch (...) {
            logger::error("Unknown failure"sv);
        }
    };

    logger::info("Reading .toml files");

    auto baseToml = std::filesystem::path(basecfg);
    readToml(baseToml);
    if (std::filesystem::is_directory(path)) {
        for (const auto& file : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_regular_file(file) && file.path().extension() == ext) {
                auto filePath = file.path();
                if (filePath != basecfg) {
                    readToml(filePath);
                }
            }
        }
    }

    logger::info("Success");

    return;

}

/* the main class for the mod. Contains Poise ctor, GetSingleton, all hooks,
   Calc. Max Poise and Calc. Poise damage, etc. Everything essential to the mod.
*/
Loki_PoiseMod::Loki_PoiseMod() {

    Loki_PoiseMod::ReadPoiseTOML();

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

/* returns the singleton for the PoiseMod class. 
   Use this instead of create a new instance.
*/
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
    REL::Relocation<std::uintptr_t> CharacterVtbl{ REL::ID(261397) };  // 165DA40
    _HandleHealthDamage_Character = CharacterVtbl.write_vfunc(0x104, HandleHealthDamage_Character);

    REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::Offset::PlayerCharacter::Vtbl };
    _HandleHealthDamage_PlayerCharacter = PlayerCharacterVtbl.write_vfunc(0x104, HandleHealthDamage_PlayerCharacter);
}

/*
    a_result = (creature->GetWeight() x creatureMul) x blockedMul;
    a_result = ((weaponWeight x weaponTypeMul x effectMul) x blockedMul) x hyperArmrMul
*/
float Loki_PoiseMod::CalculatePoiseDamage(RE::HitData& a_hitData, RE::Actor* a_actor) {

    // this whole function is BAD and DIRTY but i cant think of any other way at the moment

    auto ptr = Loki_PoiseMod::GetSingleton();

    bool bolk;
    bool atak;
    a_actor->GetGraphVariableBool("IsBlocking", bolk);
    a_actor->GetGraphVariableBool("IsAttacking", atak);
    auto aggressor = a_hitData.aggressor.get();

    for (auto idx : ptr->poiseRaceMap) {
        if (aggressor && (aggressor->race->formID == idx.first->formID)) {
            auto result = aggressor->GetWeight();
            if (bolk) { return (result * idx.second[1]) * ptr->BlockedMult; };
            if (atak) { return (result * idx.second[1]) * ptr->HyperArmourMult; };
            return (result * idx.second[1]);
        }
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

/* a_result = (creatureWeight x creatureMul) x effectMul;
   a_result = (equippedWeight + (heavyArmourskill x 0.20)) x effectMul;
*/
float Loki_PoiseMod::CalculateMaxPoise(RE::Actor* a_actor) {

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

/* Removes ragdolling in favour of animated knockdown.
   Appropriate direction and NPCs both work, tho NPCs is not
   recommended to be used.
*/
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
            } 
            else {
                str = ptr->poiseLargestBwd;
            }
            Loki_TrueHUDControl::GetSingleton()->g_trueHUD->
                FlashActorSpecialBar(SKSE::GetPluginHandle(), a_this->GetHandle(), true);
            a_this->NotifyAnimationGraph(str);
            return false;
        }
    } 
    else {
        if (ptr->NPCRagdollReplacer) {
            float knockdownDirection = 0.00f;
            a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);
            if (knockdownDirection > 0.25f && knockdownDirection < 0.75f) {
                str = ptr->poiseLargestFwd;
            } 
            else {
                str = ptr->poiseLargestBwd;
            }
            Loki_TrueHUDControl::GetSingleton()->g_trueHUD->
                FlashActorSpecialBar(SKSE::GetPluginHandle(), a_this->GetHandle(), true);
            a_this->NotifyAnimationGraph(str);
            return false;
        }
    }
    return _IsActorKnockdown(a_this, a_unk);

}

/* Used for assigning poise values to Player and NPCs.
   Hooks GetSubmergedLevel() but doesn't do anything with the function itself.
   Return value is preserved and unmodified.
   This is a cheap way of ensuring this runs for every actor.
*/
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

/* basically if it's not a spell then we don't need to do anything at all
   and just run the original function as if nothing ever happened.
*/
void Loki_PoiseMod::HandleHealthDamage_Character(RE::Character* a_char, RE::Actor* a_attacker, float a_damage) {

    auto ptr = Loki_PoiseMod::GetSingleton();
    auto avHealth = a_char->GetActorValue(RE::ActorValue::kHealth);
    auto avParalysis = a_char->GetActorValue(RE::ActorValue::kParalysis);
    if (!a_char || !a_attacker || avHealth <= 0.05f || a_char->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _HandleHealthDamage_Character(a_char, a_attacker, a_damage); }

    using spellType = RE::MagicSystem::SpellType;

    float a_result = 0.00f;
    auto hitData = [a_attacker]() -> RE::HitData* {
        auto process = a_attacker->currentProcess;
        if (!process) { return nullptr; };
        auto middleHigh = process->middleHigh;
        if (!middleHigh) { return nullptr; };
        auto lastHitData = middleHigh->lastHitData;
        return lastHitData ? lastHitData : nullptr;
    }();
    if (!hitData) {
        a_result = 0.00f;
    }
    else {
        auto spellItem = [hitData]() -> RE::SpellItem* {
            auto attackData = hitData->attackData;
            if (!attackData) { return nullptr; };
            auto attackSpell = attackData->data.attackSpell;
            return attackSpell ? attackSpell : nullptr;
        }();
        if (!spellItem) {
            a_result = 0.00f;
        }
        else {
            if (spellItem->data.spellType == spellType::kSpell) {
                for (auto idx : spellItem->effects) {
                    if (idx) {
                        a_result += idx->cost;
                    }
                }
            }
            else { a_result = 0.00f; }

            a_char->pad0EC -= (int)a_result;
            if (a_char->pad0EC > 100000) a_char->pad0EC = 0.00f;

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
            a_char->GetGraphVariableBool(ptr->isBlocking, isBlk);
            a_char->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            static RE::BSFixedString str = NULL;
            if ((float)a_char->pad0EC <= 0.00f) {
                Loki_TrueHUDControl::GetSingleton()->g_trueHUD->
                    FlashActorSpecialBar(SKSE::GetPluginHandle(), a_char->GetHandle(), false);
                a_char->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
                if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_char->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                    a_char->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargestFwd;
                        } 
                        else {
                            str = ptr->poiseLargestBwd;
                        }
                        a_char->NotifyAnimationGraph(str);         // if those, play tier 4
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseMedFwd;
                        } 
                        else {
                            str = ptr->poiseMedBwd;
                        }
                        a_char->NotifyAnimationGraph(str);  // if not those, play tier 2
                    }
                }
            }
            else if ((float)a_char->pad0EC < prcnt25 || (float)a_char->pad0EC < 2.00f) {
                if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                    a_char->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargeFwd;
                        } 
                        else {
                            str = ptr->poiseLargeBwd;
                        }
                        a_char->NotifyAnimationGraph(str);
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseMedFwd;
                        } 
                        else {
                            str = ptr->poiseMedBwd;
                        }
                        isBlk ? hitData->pushBack = 7.5f : a_char->NotifyAnimationGraph(str);
                    }
                }
            }
            else if ((float)a_char->pad0EC < prcnt35 || (float)a_char->pad0EC < 5.00f) {
                if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                    a_char->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargeFwd;
                        } 
                        else {
                            str = ptr->poiseLargeBwd;
                        }
                        a_char->NotifyAnimationGraph(str);
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseSmallFwd;
                        } 
                        else {
                            str = ptr->poiseSmallBwd;
                        }
                        isBlk ? hitData->pushBack = 5.0f : a_char->NotifyAnimationGraph(str);
                    }
                }
            }
            else if ((float)a_char->pad0EC < prcnt50 || (float)a_char->pad0EC < 10.00f) {
                if (a_char->HasKeyword(ptr->kCreature) || a_char->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_char->SetGraphVariableFloat(ptr->staggerMagn, 0.25f);
                    a_char->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseSmallFwd;
                    } 
                    else {
                        str = ptr->poiseSmallBwd;
                    }
                    isBlk ? hitData->pushBack = 2.50f : a_char->NotifyAnimationGraph(str);
                }
            }
        }
    };

    return _HandleHealthDamage_Character(a_char, a_attacker, a_damage);

}

/* basically if it's not a spell then we don't need to do anything at all
   and just run the original function as if nothing ever happened.
*/
void Loki_PoiseMod::HandleHealthDamage_PlayerCharacter(RE::PlayerCharacter* a_playerChar, RE::Actor* a_attacker, float a_damage) {

    auto ptr = Loki_PoiseMod::GetSingleton();
    auto avHealth = a_playerChar->GetActorValue(RE::ActorValue::kHealth);
    auto avParalysis = a_playerChar->GetActorValue(RE::ActorValue::kParalysis);
    if (!a_playerChar || !a_attacker || avHealth <= 0.05f || a_playerChar->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _HandleHealthDamage_PlayerCharacter(a_playerChar, a_attacker, a_damage); }

    using spellType = RE::MagicSystem::SpellType;

    float a_result = 0.00f;
    auto hitData = [a_attacker]() -> RE::HitData* {
        auto process = a_attacker->currentProcess;
        if (!process) { return nullptr; };
        auto middleHigh = process->middleHigh;
        if (!middleHigh) { return nullptr; };
        auto lastHitData = middleHigh->lastHitData;
        return lastHitData ? lastHitData : nullptr;
    }();
    if (!hitData) {
        a_result = 0.00f;
    } 
    else {
        auto spellItem = [hitData]() -> RE::SpellItem* {
            auto attackData = hitData->attackData;
            if (!attackData) { return nullptr; };
            auto attackSpell = attackData->data.attackSpell;
            return attackSpell ? attackSpell : nullptr;
        }();
        if (!spellItem) {
            a_result = 0.00f;
        } 
        else {
            if (spellItem->data.spellType == spellType::kSpell) {
                for (auto idx : spellItem->effects) {
                    if (idx) {
                        a_result += idx->cost;
                    }
                }
            } else { a_result = 0.00f; }

            a_playerChar->pad0EC -= (int)a_result;
            if (a_playerChar->pad0EC > 100000) a_playerChar->pad0EC = 0.00f;

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
            a_playerChar->GetGraphVariableBool(ptr->isBlocking, isBlk);
            a_playerChar->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
            static RE::BSFixedString str = NULL;
            if ((float)a_playerChar->pad0EC <= 0.00f) {
                Loki_TrueHUDControl::GetSingleton()->g_trueHUD->
                    FlashActorSpecialBar(SKSE::GetPluginHandle(), a_playerChar->GetHandle(), false);
                a_playerChar->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
                if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                    a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargestFwd;
                        } 
                        else {
                            str = ptr->poiseLargestBwd;
                        }
                        a_playerChar->NotifyAnimationGraph(str);         // if those, play tier 4
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseMedFwd;
                        } 
                        else {
                            str = ptr->poiseMedBwd;
                        }
                        a_playerChar->NotifyAnimationGraph(str);  // if not those, play tier 2
                    }
                }
            } 
            else if ((float)a_playerChar->pad0EC < prcnt25 || (float)a_playerChar->pad0EC < 2.00f) {
                if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                    a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargeFwd;
                        } 
                        else {
                            str = ptr->poiseLargeBwd;
                        }
                        a_playerChar->NotifyAnimationGraph(str);
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseMedFwd;
                        } 
                        else {
                            str = ptr->poiseMedBwd;
                        }
                        isBlk ? hitData->pushBack = 7.5f : a_playerChar->NotifyAnimationGraph(str);
                    }
                }
            } 
            else if ((float)a_playerChar->pad0EC < prcnt35 || (float)a_playerChar->pad0EC < 5.00f) {
                if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                    a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (hitData->flags == RE::HitData::Flag::kExplosion) {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseLargeFwd;
                        } 
                        else {
                            str = ptr->poiseLargeBwd;
                        }
                        a_playerChar->NotifyAnimationGraph(str);
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseSmallFwd;
                        } 
                        else {
                            str = ptr->poiseSmallBwd;
                        }
                        isBlk ? hitData->pushBack = 5.0f : a_playerChar->NotifyAnimationGraph(str);
                    }
                }
            } 
            else if ((float)a_playerChar->pad0EC < prcnt50 || (float)a_playerChar->pad0EC < 10.00f) {
                if (a_playerChar->HasKeyword(ptr->kCreature) || a_playerChar->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                    a_playerChar->SetGraphVariableFloat(ptr->staggerMagn, 0.25f);
                    a_playerChar->NotifyAnimationGraph(ptr->ae_Stagger);
                } 
                else {
                    if (stagDir > 0.25f && stagDir < 0.75f) {
                        str = ptr->poiseSmallFwd;
                    } 
                    else {
                        str = ptr->poiseSmallBwd;
                    }
                    isBlk ? hitData->pushBack = 2.50f : a_playerChar->NotifyAnimationGraph(str);
                }
            }
        }
    };

    return _HandleHealthDamage_PlayerCharacter(a_playerChar, a_attacker, a_damage);

}

/* The main function of Poise, subtracting poise damage from health and triggering animations.
   Creatures end up using normal stagger behaviours, but anything else uses the new 
   poise behaviours and animations. 
*/
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
        Loki_TrueHUDControl::GetSingleton()->g_trueHUD->
            FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), false);
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
        } 
        else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {  // check if explosion, dragon, giant attack or dwarven
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargestFwd;
                } 
                else {
                    str = ptr->poiseLargestBwd;
                }
                a_actor->NotifyAnimationGraph(str);         // if those, play tier 4
            } 
            else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseMedFwd;
                } 
                else {
                    str = ptr->poiseMedBwd;
                }
                a_actor->NotifyAnimationGraph(str);  // if not those, play tier 2
            }
        }
    } 
    else if ((float)a_actor->pad0EC < prcnt25 || (float)a_actor->pad0EC < 2.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } 
        else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {  // check if explosion, dragon, giant attack or dwarven
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargeFwd;
                } 
                else {
                    str = ptr->poiseLargeBwd;
                }
                a_actor->NotifyAnimationGraph(str);           // if those, play tier 3
            } 
            else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseMedFwd;
                } 
                else {
                    str = ptr->poiseMedBwd;
                }
                isBlk ? a_hitData.pushBack = 5.00 : a_actor->NotifyAnimationGraph(str); // if block, set pushback, ! play tier 2
            }
        }
    } 
    else if ((float)a_actor->pad0EC < prcnt35 || (float)a_actor->pad0EC < 5.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) {
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } 
        else {
            if (a_hitData.flags == HitFlag::kExplosion || a_hitData.aggressor.get()->HasKeyword(ptr->kDragon)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kGiant) || a_hitData.aggressor.get()->HasKeyword(ptr->kDwarven)
                || a_hitData.aggressor.get()->HasKeyword(ptr->kTroll) || a_hitData.aggressor.get()->race->formID == kLurker) {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseLargeFwd;
                } 
                else {
                    str = ptr->poiseLargeBwd;
                }
                a_actor->NotifyAnimationGraph(str);  // play tier 3 again
            } 
            else {
                if (stagDir > 0.25f && stagDir < 0.75f) {
                    str = ptr->poiseSmallFwd;
                } 
                else {
                    str = ptr->poiseSmallBwd;
                }
                isBlk ? a_hitData.pushBack = 3.75 : a_actor->NotifyAnimationGraph(str);
            }
        }
    } 
    else if ((float)a_actor->pad0EC < prcnt50 || (float)a_actor->pad0EC < 10.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) {
            a_actor->SetGraphVariableFloat(ptr->staggerMagn, 0.25f);
            a_actor->NotifyAnimationGraph(ptr->ae_Stagger);
        } 
        else {
            if (stagDir > 0.25f && stagDir < 0.75f) {
                str = ptr->poiseSmallFwd;
            } 
            else {
                str = ptr->poiseSmallBwd;
            }
            isBlk ? a_hitData.pushBack = 2.50f : a_actor->NotifyAnimationGraph(str);
        }
    }

    return _ProcessHitEvent(a_actor, a_hitData);

}