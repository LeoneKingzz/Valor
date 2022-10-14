#pragma once
#include "PCH.h"
#include <shared_mutex>

class perilous
{
public:
	static perilous* GetSingleton()
	{
		static perilous singleton;
		return &singleton;
	}

	void init();

	void attempt_start_perilous_attack(RE::Actor* a_actor);

	void attempt_end_perilous_attack(RE::Actor* a_actor);

private:

	void perform_perilous_attack(RE::Actor* a_actor, RE::ActorHandle a_target);

	bool is_perilous_attacking(RE::Actor* a_actor, RE::ActorHandle& r_target);

	RE::SpellItem* perilousSpell;
	RE::BGSSoundDescriptorForm* perilousSound;
	RE::TESEffectShader* temp;
	RE::BGSArtObject* perilousHitEffectArt;

	#define MAX_PERILOUS_ATTACKER 2

	std::unordered_map<RE::ActorHandle, uint32_t> perilous_counter; /*Counter to keep track of an actor's current perilous attackers, to prevent being surrounded by too many perilous atks.*/
	mutable std::shared_mutex perilous_counter_lock;
	
	std::unordered_map<RE::ActorHandle, RE::ActorHandle> perilous_attacks; /*Map of Attacker->target to keep track of all ongoing perilous attacks.*/
	mutable std::shared_mutex perilous_attacks_lock;
};
