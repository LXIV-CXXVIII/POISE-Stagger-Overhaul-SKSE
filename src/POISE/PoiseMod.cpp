#include "PoiseMod.h"

void Loki::PoiseMod::ReadPoiseTOML() {

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
                logger::info("FormID -> {x}", *formID);
                auto plugin = raceTable["Plugin"].value<std::string_view>();
                logger::info("plugin -> {}", *plugin);
                auto race = dataHandle->LookupForm<RE::TESRace>(*formID, *plugin);

                auto poiseValues = raceTable["PoiseValues"].as_array();
                if (poiseValues) {
                    std::vector<float> vals = {};
                    for (auto& value : *poiseValues) {
                        logger::info("value -> {}", *value.value<float>());
                        vals.push_back(*value.value<float>());
                    }
                    poiseRaceMap.insert_or_assign(race ? race : nullptr, vals);
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

Loki::PoiseMod::PoiseMod() {

    Loki::PoiseMod::ReadPoiseTOML();

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

    if (auto dataHandle = RE::TESDataHandler::GetSingleton(); dataHandle) {
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

Loki::PoiseMod* Loki::PoiseMod::GetSingleton() {
    static Loki::PoiseMod* singleton = new Loki::PoiseMod();
    return singleton;
}

void Loki::PoiseMod::InstallStaggerHook() {
    REL::Relocation<std::uintptr_t> StaggerHook{ REL::ID(37673/*628c20*/) };

    auto& trampoline = SKSE::GetTrampoline();
    _ProcessHitEvent = trampoline.write_call<5>(StaggerHook.address() + 0x3C0, ProcessHitEvent);

    logger::info("ProcessHitEvent hook injected");
}

void Loki::PoiseMod::InstallWaterHook() {
    REL::Relocation<std::uintptr_t> ActorUpdate{ REL::ID(36357) };
    // last ditch effort
    auto& trampoline = SKSE::GetTrampoline();
    _GetSubmergedLevel = trampoline.write_call<5>(ActorUpdate.address() + 0x6D3, GetSubmergedLevel);

    logger::info("Update hook injected");
}

void Loki::PoiseMod::InstallIsActorKnockdownHook() {
    REL::Relocation<std::uintptr_t> isActorKnockdown{ REL::ID(38858) };

    auto& trampoline = SKSE::GetTrampoline();
    _IsActorKnockdown = trampoline.write_call<5>(isActorKnockdown.address() + 0x7E, IsActorKnockdown);

    logger::info("isActorKnockdown hook injected");
}

void Loki::PoiseMod::InstallMagicEventSink() {
    auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
    if (sourceHolder) { sourceHolder->AddEventSink(PoiseMagicDamage::GetSingleton()); }
}

/*
* im keeping this here because i always forget how to hook vfuncs
void Loki::PoiseMod::InstallVFuncHooks() {
    REL::Relocation<std::uintptr_t> CharacterVtbl{ REL::ID(261397) };  // 165DA40
    _HandleHealthDamage_Character = CharacterVtbl.write_vfunc(0x104, HandleHealthDamage_Character);

    REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::Offset::PlayerCharacter::Vtbl };
    _HandleHealthDamage_PlayerCharacter = PlayerCharacterVtbl.write_vfunc(0x104, HandleHealthDamage_PlayerCharacter);
}
*/

Loki::PoiseMagicDamage* Loki::PoiseMagicDamage::GetSingleton() {
    static PoiseMagicDamage singleton;
    return &singleton;
}

auto Loki::PoiseMagicDamage::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) -> RE::BSEventNotifyControl {
    if (!a_event || !a_event->projectile) {
        return RE::BSEventNotifyControl::kContinue;
    }
    if (!a_event->target || !a_event->cause) {
        return RE::BSEventNotifyControl::kContinue;
    } 
    else {
        if (auto projectile = RE::TESForm::LookupByID(a_event->projectile)->As<RE::Projectile>(); projectile) {
            if (auto actor = a_event->target.get()->As<RE::Actor>(); actor) {
                auto ptr = Loki::PoiseMod::GetSingleton();

                float a_result = 8.00f;
                if (auto effect = projectile->spell->avEffectSetting; effect) {
                    a_result = effect->data.baseCost;
                    RE::ConsoleLog::GetSingleton()->Print("Poise Damage from Spell -> %f", a_result);
                }
                else {
                    RE::ConsoleLog::GetSingleton()->Print("avEffectSetting null, using default poise damage");
                }
                actor->pad0EC -= (int)a_result;
                if (actor->pad0EC > 100000) { actor->pad0EC = 0.00f; }

                RE::ConsoleLog::GetSingleton()->Print("current poise health -> %f", actor->pad0EC);

                float maxPoise = PoiseMod::CalculateMaxPoise(actor);
                auto prcnt25 = maxPoise * 0.25f;
                auto prcnt35 = maxPoise * 0.35f;
                auto prcnt50 = maxPoise * 0.50f;

                auto Form = RE::TESForm::LookupByID(a_event->source)->As<RE::Actor>();
                auto hitPos = Form->GetPosition();
                auto heading = actor->GetHeadingAngle(hitPos, false);
                auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
                if (actor->GetHandle() == Form->GetHandle()) { stagDir = 0.0f; } // 0 when self-hit

                auto caster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
                caster->Cast(ptr->poiseDelaySpell, false, actor, 1.0f, false, 0.0f, 0);

                bool isBlk = false;
                static RE::BSFixedString str = NULL;
                if ((float)actor->pad0EC <= 0.00f) {
                    actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
                    actor->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
                    if (TrueHUDControl::GetSingleton()->g_trueHUD) {
                        TrueHUDControl::GetSingleton()->g_trueHUD->
                            FlashActorSpecialBar(SKSE::GetPluginHandle(), actor->GetHandle(), false);
                    }
                    if (actor->HasKeyword(ptr->kCreature) || actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                        actor->SetGraphVariableFloat(ptr->staggerMagn, 1.00f);
                        actor->NotifyAnimationGraph(ptr->ae_Stagger);          // play animation
                    } 
                    else {
                        if (projectile->explosion) {
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseLargestFwd;
                            } 
                            else {
                                str = ptr->poiseLargestBwd;
                            }
                            actor->NotifyAnimationGraph(str);         // if those, play tier 4
                        } else {
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseMedFwd;
                            } 
                            else {
                                str = ptr->poiseMedBwd;
                            }
                            actor->NotifyAnimationGraph(str);  // if not those, play tier 2
                        }
                    }
                }
                else if ((float)actor->pad0EC <= prcnt25 || (float)actor->pad0EC <= 2.00f) {
                    actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
                    if (actor->HasKeyword(ptr->kCreature) || actor->HasKeyword(ptr->kDwarven)) { // if creature, use normal beh
                        actor->SetGraphVariableFloat(ptr->staggerMagn, 0.75f);
                        actor->NotifyAnimationGraph(ptr->ae_Stagger);
                    } 
                    else {
                        if (projectile->explosion) {  // check if explosion, dragon, giant attack or dwarven
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseLargeFwd;
                            } 
                            else {
                                str = ptr->poiseLargeBwd;
                            }
                            actor->NotifyAnimationGraph(str);           // if those, play tier 3
                        } 
                        else {
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseMedFwd;
                            } 
                            else {
                                str = ptr->poiseMedBwd;
                            }
                            isBlk ? NULL : actor->NotifyAnimationGraph(str); // if block, set pushback, ! play tier 2
                        }
                    }
                }
                else if ((float)actor->pad0EC <= prcnt35 || (float)actor->pad0EC <= 5.00f) {
                    actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
                    if (actor->HasKeyword(ptr->kCreature) || actor->HasKeyword(ptr->kDwarven)) {
                        actor->SetGraphVariableFloat(ptr->staggerMagn, 0.50f);
                        actor->NotifyAnimationGraph(ptr->ae_Stagger);
                    } 
                    else {
                        if (projectile->explosion) {
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseLargeFwd;
                            } 
                            else {
                                str = ptr->poiseLargeBwd;
                            }
                            actor->NotifyAnimationGraph(str);  // play tier 3 again
                        } 
                        else {
                            if (stagDir > 0.25f && stagDir < 0.75f) {
                                str = ptr->poiseSmallFwd;
                            } 
                            else {
                                str = ptr->poiseSmallBwd;
                            }
                            isBlk ? NULL : actor->NotifyAnimationGraph(str);
                        }
                    }
                }
                else if ((float)actor->pad0EC <= prcnt50 || (float)actor->pad0EC <= 10.00f) {
                    actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
                    if (actor->HasKeyword(ptr->kCreature) || actor->HasKeyword(ptr->kDwarven)) {
                        actor->SetGraphVariableFloat(ptr->staggerMagn, 0.25f);
                        actor->NotifyAnimationGraph(ptr->ae_Stagger);
                    } 
                    else {
                        if (stagDir > 0.25f && stagDir < 0.75f) {
                            str = ptr->poiseSmallFwd;
                        } 
                        else {
                            str = ptr->poiseSmallBwd;
                        }
                        isBlk ? NULL : actor->NotifyAnimationGraph(str);
                    }
                }
            }
        }
    }
    if (a_event->target->IsPlayerRef()) {
        RE::ConsoleLog::GetSingleton()->Print("formID -> %11x", a_event->projectile);
    }
    return RE::BSEventNotifyControl::kContinue;
}

/*
    a_result = (creature->GetWeight() x creatureMul) x blockedMul;
    a_result = ((weaponWeight x weaponTypeMul x effectMul) x blockedMul) x hyperArmrMul
*/
float Loki::PoiseMod::CalculatePoiseDamage(RE::HitData& a_hitData, RE::Actor* a_actor) {

    // this whole function is BAD and DIRTY but i cant think of any other way at the moment

    auto ptr = Loki::PoiseMod::GetSingleton();

    bool blk, atk;
    a_actor->GetGraphVariableBool("IsBlocking", blk);
    a_actor->GetGraphVariableBool("IsAttacking", atk);
    auto aggressor = a_hitData.aggressor.get().get();

    auto weap = a_hitData.weapon;
    float a_result = 0.00f;
    if (!weap) {
        auto attacker = a_hitData.aggressor.get();
        if (!attacker) {
            a_result = 8.00f;
        }
        else {
            auto attackerWeap = attacker->GetAttackingWeapon();
            if (!attackerWeap) {
                a_result = 8.00f;
            }
            else {
                a_result = attackerWeap->GetWeight();
            }
        }
    }
    else {
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
    }

    for (auto idx : ptr->poiseRaceMap) {
        if (aggressor) {
            RE::TESRace* a_actorRace = aggressor->race;
            RE::TESRace* a_mapRace = idx.first;
            if (aggressor && a_actorRace && a_mapRace) {
                if (a_actorRace->formID == a_mapRace->formID) {
                    if (aggressor->HasKeyword(ptr->kCreature) || aggressor->HasKeyword(ptr->kDwarven)) {
                        a_result = idx.second[1];
                    }
                    else {
                        a_result *= idx.second[1];
                    }
                    break;
                }
            }
        }
    }

    RE::BSFixedString buffKeyword = "PoiseDmgBuff";
    RE::BSFixedString nerfKeyword = "PoiseDmgNerf";

    auto hasBuff = PluginTools::ActorHasEffectWithKeyword(aggressor, buffKeyword);
    if (hasBuff) {
        logger::info("damage buff keyword detected");
        auto buffPercent = hasBuff->effectItem.magnitude / 100.00f; // convert to percentage
        auto resultingBuff = (a_result * buffPercent);
        a_result += resultingBuff; // aggressor has buff that makes them do more poise damage
    }
    auto hasNerf = PluginTools::ActorHasEffectWithKeyword(aggressor, nerfKeyword);
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

float Loki::PoiseMod::CalculateMaxPoise(RE::Actor* a_actor) {

    auto ptr = Loki::PoiseMod::GetSingleton();

    float a_result = (a_actor->equippedWeight + (a_actor->GetBaseActorValue(RE::ActorValue::kHeavyArmor) * 0.20f));

    for (auto idx : ptr->poiseRaceMap) {
        if (a_actor) {
            RE::TESRace* a_actorRace = a_actor->race;
            RE::TESRace* a_mapRace = idx.first;
            if (a_actorRace && a_mapRace) {
                if (a_actorRace->formID == a_mapRace->formID) {
                    if (a_actor->HasKeyword(ptr->kCreature) || a_actor->HasKeyword(ptr->kDwarven)) {
                        a_result = idx.second[0];
                    } 
                    else {
                        a_result *= idx.second[0];
                    }
                    break;
                }
            }
        }
    }

    RE::BSFixedString buffKeyword = "MaxPoiseBuff";
    RE::BSFixedString nerfKeyword = "MaxPoiseNerf";

    auto hasBuff = PluginTools::ActorHasEffectWithKeyword(a_actor, buffKeyword);
    if (hasBuff) {
        logger::info("health buff keyword detected");
        auto buffPercent = hasBuff->effectItem.magnitude / 100.00f; // convert to percentage
        auto resultingBuff = (a_result * buffPercent);
        a_result += resultingBuff;
    }
    auto hasNerf = PluginTools::ActorHasEffectWithKeyword(a_actor, nerfKeyword);
    if (hasNerf) {
        logger::info("health nerf keyword detected");
        auto nerfPercent = hasNerf->effectItem.magnitude / 100.00f;
        auto resultingNerf = (a_result * nerfPercent);
        a_result -= resultingNerf;
    }

    return a_result;

}

bool Loki::PoiseMod::IsActorKnockdown(RE::Character* a_this, std::int64_t a_unk) {

    auto ptr = Loki::PoiseMod::GetSingleton();

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
            if (TrueHUDControl::GetSingleton()->g_trueHUD) {
                TrueHUDControl::GetSingleton()->g_trueHUD->
                    FlashActorSpecialBar(SKSE::GetPluginHandle(), a_this->GetHandle(), true);
            }
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
            if (TrueHUDControl::GetSingleton()->g_trueHUD) {
                TrueHUDControl::GetSingleton()->g_trueHUD->
                    FlashActorSpecialBar(SKSE::GetPluginHandle(), a_this->GetHandle(), true);
            }
            a_this->NotifyAnimationGraph(str);
            return false;
        }
    }
    return _IsActorKnockdown(a_this, a_unk);

}

float Loki::PoiseMod::GetSubmergedLevel(RE::Actor* a_actor, float a_zPos, RE::TESObjectCELL* a_cell) {

    auto ptr = Loki::PoiseMod::GetSingleton();

    auto avHealth = a_actor->GetActorValue(RE::ActorValue::kHealth);
    if (avHealth <= 0.05f || !ptr->PoiseRegenEnabled) { return _GetSubmergedLevel(a_actor, a_zPos, a_cell); }

    if (!a_actor->HasMagicEffect(ptr->poiseDelayEffect)) {
        auto a_result = (int)CalculateMaxPoise(a_actor);
        a_actor->pad0EC = a_result;
        if (a_actor->pad0EC > 100000) { a_actor->pad0EC = 0.00f; }
    }

    return _GetSubmergedLevel(a_actor, a_zPos, a_cell);

}

void Loki::PoiseMod::ProcessHitEvent(RE::Actor* a_actor, RE::HitData& a_hitData) {

    auto ptr = Loki::PoiseMod::GetSingleton();

    using HitFlag = RE::HitData::Flag;
    RE::FormID kLurker = 0x14495;

    auto avHealth = a_actor->GetActorValue(RE::ActorValue::kHealth);
    auto avParalysis = a_actor->GetActorValue(RE::ActorValue::kParalysis);
    if (avHealth <= 0.05f || a_actor->IsInKillMove() || avParalysis || !ptr->PoiseSystemEnabled) { return _ProcessHitEvent(a_actor, a_hitData); }

    float dmg = CalculatePoiseDamage(a_hitData, a_actor);

    if (dmg <= 0.00f) dmg = 0.00f;
    a_actor->pad0EC -= (int)dmg;  // there was some bullshit with integer underflow
    if (a_actor->pad0EC > 100000) a_actor->pad0EC = 0.00f;  // this fixed it...
    if (ptr->ConsoleInfoDump) {
        RE::ConsoleLog::GetSingleton()->Print("---");
        RE::ConsoleLog::GetSingleton()->Print("Aggessor's Weight: %f", a_hitData.aggressor.get()->GetWeight());
        RE::ConsoleLog::GetSingleton()->Print("Aggressor's Current Poise Health: %d", a_hitData.aggressor.get()->pad0EC);
        RE::ConsoleLog::GetSingleton()->Print("Aggresssor's Max Poise Health: %f", CalculateMaxPoise(a_hitData.aggressor.get().get()));
        RE::ConsoleLog::GetSingleton()->Print("Aggressor's Poise Damage: %f", dmg);
        RE::ConsoleLog::GetSingleton()->Print("-");
        RE::ConsoleLog::GetSingleton()->Print("Victim's Weight: %f", a_actor->GetWeight());
        RE::ConsoleLog::GetSingleton()->Print("Victim's Current Poise Health: %d", a_actor->pad0EC);
        RE::ConsoleLog::GetSingleton()->Print("Victim's Max Poise Health %f", CalculateMaxPoise(a_actor));
        RE::ConsoleLog::GetSingleton()->Print("---");
    }

    auto hitPos = a_hitData.aggressor.get()->GetPosition();
    auto heading = a_actor->GetHeadingAngle(hitPos, false);
    auto stagDir = (heading >= 0.0f) ? heading / 360.0f : (360.0f + heading) / 360.0f;
    if (a_actor->GetHandle() == a_hitData.aggressor) { stagDir = 0.0f; } // 0 when self-hit

    auto a = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
    a->Cast(ptr->poiseDelaySpell, false, a_actor, 1.0f, false, 0.0f, 0);

    float maxPoise = CalculateMaxPoise(a_actor);
    auto prcnt25 = maxPoise * 0.25f;
    auto prcnt35 = maxPoise * 0.35f;
    auto prcnt50 = maxPoise * 0.50f;

    bool isBlk = false;
    static RE::BSFixedString str = NULL;
    a_actor->GetGraphVariableBool(ptr->isBlocking, isBlk);
    if ((float)a_actor->pad0EC <= 0.00f) {
        a_actor->SetGraphVariableFloat(ptr->staggerDire, stagDir); // set direction
        a_actor->pad0EC = maxPoise; // remember earlier when we calculated max poise health?
        if (TrueHUDControl::GetSingleton()->g_trueHUD) {
            TrueHUDControl::GetSingleton()->g_trueHUD->
                FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), false);
        }
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