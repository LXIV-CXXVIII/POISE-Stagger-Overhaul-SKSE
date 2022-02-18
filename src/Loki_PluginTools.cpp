#include "Loki_PluginTools.h"

void* Loki_PluginTools::CodeAllocation(Xbyak::CodeGenerator& a_code, SKSE::Trampoline* t_ptr) {

    auto result = t_ptr->allocate(a_code.getSize());
    std::memcpy(result, a_code.getCode(), a_code.getSize());
    return result;

}

float Loki_PluginTools::lerp(float a, float b, float f) {

    return a + f * (b - a);

}

bool Loki_PluginTools::WeaponHasKeyword(RE::TESObjectWEAP* a_weap, RE::BSFixedString a_editorID) {

    if (a_weap->keywords) {
        for (std::uint32_t idx = 0; idx < a_weap->numKeywords; ++idx) {
            if (a_weap->keywords[idx] && a_weap->keywords[idx]->formEditorID == a_editorID) {
                return true;
            }
        }
    }

    return false;

}

RE::Effect* Loki_PluginTools::ActorHasEffectWithKeyword(RE::Actor* a_actor, RE::BSFixedString a_editorID) {

    auto activeEffect = a_actor->GetActiveEffectList();
    if (activeEffect) {
        for (auto& ae : *activeEffect) {
            if (!ae->effect) {
                return NULL;
            }
            if (!ae->effect->baseEffect) {
                return NULL;
            }
            auto keywords = ae->effect->baseEffect->keywords;
            if (!keywords) {
                return NULL;
            }
            for (std::uint32_t idx = 0; idx < ae->effect->baseEffect->numKeywords; ++idx) {
                if (keywords[idx] && keywords[idx]->formEditorID == a_editorID) {
                    return ae->effect ? ae->effect : nullptr;
                }
            }
        }
    }
    return NULL;

}