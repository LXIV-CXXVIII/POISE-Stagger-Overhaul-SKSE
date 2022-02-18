#pragma once

class Loki_PluginTools {
public:
	static void* CodeAllocation(Xbyak::CodeGenerator& a_code, SKSE::Trampoline* t_ptr);
	static float lerp(float a, float b, float f);
	static bool WeaponHasKeyword(RE::TESObjectWEAP* a_weap, RE::BSFixedString a_editorID);
	static RE::Effect* ActorHasEffectWithKeyword(RE::Actor* a_actor, RE::BSFixedString a_editorID);

private:

protected:

};