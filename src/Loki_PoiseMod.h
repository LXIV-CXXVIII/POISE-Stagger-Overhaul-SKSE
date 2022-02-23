#pragma once
#include "Loki_TrueHUDControl.h"

class Loki_PoiseMod {

public:
    bool ConsoleInfoDump;

    float BowMult, CrossbowMult, Hand2Hand, OneHandAxe, OneHandDagger, OneHandMace, OneHandSword, TwoHandAxe, TwoHandSword;
    float RapierMult, PikeMult, SpearMult, HalberdMult, QtrStaffMult, CaestusMult, ClawMult, WhipMult;
    float PowerAttackMult, BlockedMult, BashMult, HyperArmourMult;
    float CreaturePoiseHealthMult, DragonPoiseHealthMult, DwarvenPoiseHealthMult;
    float CreaturePoiseDamageMult, DragonPoiseDamageMult, DwarvenPoiseDamageMult;
    bool PoiseSystemEnabled, PlayerRagdollReplacer, NPCRagdollReplacer, PoiseRegenEnabled, TrueHUDBars;

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

    std::unordered_map<RE::BGSKeyword*, float*> healthKywdMap = {};
    std::unordered_map<RE::BGSKeyword*, float*> damageKywdMap = {};
    std::unordered_map<RE::TESRace*, float*> poiseRaceMap;

    static void ReadPoiseIni(const wchar_t* a_filename, std::unordered_map<RE::TESRace*, float*> a_map);
    Loki_PoiseMod();
    static Loki_PoiseMod* GetSingleton();

    static void InstallStaggerHook();
    static void InstallWaterHook();
    static void InstallIsActorKnockdownHook();
    static void InstallVFuncHooks();

private:
    static float CalculatePoiseDamage(RE::HitData& a_hitData, RE::Actor* a_actor);
    static float CalculateMaxPoise(RE::Actor* a_actor);
    static bool IsActorKnockdown(RE::Character* a_this, std::int64_t a_unk);
    static float GetSubmergedLevel(RE::Actor* a_actor, float a_zPos, RE::TESObjectCELL* a_cell);
    static void HandleHealthDamage_Character(RE::Character* a_char, RE::Actor* a_attacker, float a_damage);
    static void HandleHealthDamage_PlayerCharacter(RE::PlayerCharacter* a_playerChar, RE::Actor* a_attacker, float a_damage);
    static void ProcessHitEvent(RE::Actor* a_actor, RE::HitData& a_hitData);

    static inline REL::Relocation<decltype(GetSubmergedLevel)> _GetSubmergedLevel;
    static inline REL::Relocation<decltype(ProcessHitEvent)> _ProcessHitEvent;
    static inline REL::Relocation<decltype(HandleHealthDamage_Character)> _HandleHealthDamage_Character;
    static inline REL::Relocation<decltype(HandleHealthDamage_PlayerCharacter)> _HandleHealthDamage_PlayerCharacter;
    static inline REL::Relocation<decltype(IsActorKnockdown)> _IsActorKnockdown;

protected:

};