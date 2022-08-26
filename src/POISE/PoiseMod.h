#pragma once
#include "C:/dev/simpleini-master/SimpleIni.h"
#include <toml++/toml.h>

#include "Loki_PluginTools.h"
#include "TrueHUDControl.h"

namespace Loki {

    class PoiseMagicDamage : public RE::BSTEventSink<RE::TESHitEvent> {

    public:
        static PoiseMagicDamage* GetSingleton();

        auto ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource)->RE::BSEventNotifyControl override;

    protected:
        PoiseMagicDamage() = default;
        PoiseMagicDamage(const PoiseMagicDamage&) = delete;
        PoiseMagicDamage(PoiseMagicDamage&&) = delete;
        virtual ~PoiseMagicDamage() = default;

        auto operator=(const PoiseMagicDamage&)->PoiseMagicDamage & = delete;
        auto operator=(PoiseMagicDamage&&)->PoiseMagicDamage & = delete;
    };

    class PoiseMod {

    public:
        bool ConsoleInfoDump;

        float poiseBreakThreshhold0, poiseBreakThreshhold1, poiseBreakThreshhold2;
        float BowMult, CrossbowMult, Hand2Hand, OneHandAxe, OneHandDagger, OneHandMace, OneHandSword, TwoHandAxe, TwoHandSword;
        float RapierMult, PikeMult, SpearMult, HalberdMult, QtrStaffMult, CaestusMult, ClawMult, WhipMult;
        float PowerAttackMult, BlockedMult, BashMult, MeleeHyperArmourMult, SpellHyperArmourMult;
        bool PlayerPoiseEnabled, NPCPoiseEnabled, PlayerRagdollReplacer, NPCRagdollReplacer, PoiseRegenEnabled, TrueHUDBars;

        const RE::BSFixedString ae_Stagger = "staggerStart";
        const RE::BSFixedString staggerDire = "staggerDirection";
        const RE::BSFixedString staggerMagn = "staggerMagnitude";
        const RE::BSFixedString isBlocking = "IsBlocking";
        const RE::BSFixedString isAttacking = "IsAttacking";
        const RE::BSFixedString setStaggerDire = "setstagger";

        const RE::BSFixedString poiseSmallBwd = "poise_small_start";
        const RE::BSFixedString poiseSmallFwd = "poise_small_start_fwd";

        const RE::BSFixedString poiseMedBwd = "poise_med_start";
        const RE::BSFixedString poiseMedFwd = "poise_med_start_fwd";

        const RE::BSFixedString poiseLargeBwd = "poise_large_start";
        const RE::BSFixedString poiseLargeFwd = "poise_large_start_fwd";

        const RE::BSFixedString poiseLargestBwd = "poise_largest_start";
        const RE::BSFixedString poiseLargestFwd = "poise_largest_start_fwd";

        RE::SpellItem* poiseDelaySpell = NULL;
        RE::EffectSetting* poiseDelayEffect = NULL;
        RE::BGSKeyword* kCreature = NULL;
        RE::BGSKeyword* kDragon = NULL;
        RE::BGSKeyword* kGiant = NULL;
        RE::BGSKeyword* kGhost = NULL;
        RE::BGSKeyword* kDwarven = NULL;
        RE::BGSKeyword* kTroll = NULL;
        RE::BGSKeyword* WeapMaterialSilver = NULL;

        RE::BGSKeyword* PoiseDmgNerf = NULL;
        RE::BGSKeyword* PoiseDmgBuff = NULL;
        RE::BGSKeyword* PoiseHPNerf = NULL;
        RE::BGSKeyword* PoiseHPBuff = NULL;

        static inline std::unordered_map<RE::TESRace*, std::vector<float>> poiseRaceMap;

        PoiseMod();
        static void ReadPoiseTOML();
        static PoiseMod* GetSingleton();

        static void InstallStaggerHook();
        static void InstallWaterHook();
        static void InstallIsActorKnockdownHook();
        static void InstallMagicEventSink();

        static float CalculatePoiseDamage(RE::HitData& a_hitData, RE::Actor* a_actor);
        static float CalculateMaxPoise(RE::Actor* a_actor);

    private:
        static bool IsActorKnockdown(RE::Character* a_this, std::int64_t a_unk);
        static float GetSubmergedLevel(RE::Actor* a_actor, float a_zPos, RE::TESObjectCELL* a_cell);
        static void ProcessHitEvent(RE::Actor* a_actor, RE::HitData& a_hitData);

        static inline REL::Relocation<decltype(GetSubmergedLevel)> _GetSubmergedLevel;
        static inline REL::Relocation<decltype(ProcessHitEvent)> _ProcessHitEvent;
        static inline REL::Relocation<decltype(IsActorKnockdown)> _IsActorKnockdown;

    protected:

    };

};