#include "hooks.h"

namespace hooks
{

	static inline RE::BSFixedString cPtr_bashPowerStart = "bashPowerStart";
	/// <summary>
	/// Handle perilous attacking.
	/// </summary>
	/// <param name="a_actionData"></param>
	/// <returns>Whether the attack action is performed.</returns>
	// "TKDodgeStop"_h:
	// "MCO_DodgeInitiate"_h:
	// case "MCO_Recovery"_h:
	// 	bool bIsDodging = false;
	// 	if (Utils::Actor::isHumanoid(actor) && actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
	// 		if (actor->AsActorState()->actorState2.wantBlocking) {
	// 			actor->NotifyAnimationGraph("MCO_EndAnimation");
	// 			actor->NotifyAnimationGraph("MCO_AnimStop");
	// 			actor->NotifyAnimationGraph("attackStop");
	// 			actor->NotifyAnimationGraph("blockStart");
	// 		}
	// 	}
	// 	break;
	// if (!actor->IsPlayerRef()) {
	// 	actor->NotifyAnimationGraph("recoilStop");
	// }
	// break;

	void on_animation_event::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		if (!a_event->holder) {
			return;
		}
		std::string_view eventTag = a_event->tag.data();
		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>();
		switch (hash(eventTag.data(), eventTag.size())) {
		case "TKDR_DodgeStart"_h:
		    if (!actor->IsPlayerRef()) {
				const auto StaminaCost = RE::TESForm::LookupByEditorID<RE::MagicItem>("StaminaCostSpell_UND");
				const auto caster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
				int iStep = 0;
				if (actor->GetGraphVariableInt("iStep", iStep) && iStep) {
					if (settings::bStaminaCost_enable) {
						caster->CastSpellImmediate(StaminaCost, true, actor, 1, false, -(settings::fDodgeRoll_staminacost), actor);
					}
					if (settings::biFrames_enable) {
						dodge::Set_iFrames(actor);
					}
					break;
				} else {
					if (settings::bStaminaCost_enable) {
						caster->CastSpellImmediate(StaminaCost, true, actor, 1, false, -(settings::fSideStep_staminacost), actor);
					}
					if (settings::biFrames_enable) {
						dodge::Set_iFrames(actor);
					}
				}
			}
			break;

		case "TKDR_DodgeEnd"_h:
			if (!actor->IsPlayerRef()) {
				if (settings::biFrames_enable) {
					dodge::Reset_iFrames(actor);
				}
				dodge::GetSingleton()->set_dodge_phase(actor, false);
			}
			break;
	
		// case "preHitFrame"_h:
		// 	if (!(actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash || actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kHit)) {
				
		// 		dodge::GetSingleton()->react_to_melee(actor, get_combat_reach(actor));
		// 	}
		// 	break;

		case "Voice_SpellFire_Event"_h:
			if (actor->GetCurrentShout()->variations->spell->As<RE::MagicItem>()->IsHostile()) {
				dodge::GetSingleton()->react_to_shouts_spells_fast(actor, 3000.0f);
			}
			break;

		// case "BeginCastVoice"_h:
		// 	if (actor->GetCurrentShout()->variations->spell->As<RE::MagicItem>()->IsHostile()) {
				
		// 		dodge::GetSingleton()->react_to_shouts_spells(actor, 3000.0f);
		// 	}
		// 	break;

		case "MLh_SpellFire_Event"_h:
			if (actor->GetCurrentShout()->variations->spell->As<RE::MagicItem>()->IsHostile()) {
				dodge::GetSingleton()->react_to_shouts_spells_fast(actor, 3000.0f);
			}
			break;

		// case "BeginCastLeft"_h:
		// 	if (actor->GetEquippedObject(true)->As<RE::MagicItem>()->IsHostile()) {
				
		// 		dodge::GetSingleton()->react_to_shouts_spells(actor, 2000.0f);
		// 	}
		// 	break;

		case "MRh_SpellFire_Event"_h:
			if (actor->GetCurrentShout()->variations->spell->As<RE::MagicItem>()->IsHostile()) {
				dodge::GetSingleton()->react_to_shouts_spells_fast(actor, 3000.0f);
			}
			break;

		// case "BeginCastRight"_h:
		// 	if (actor->GetEquippedObject(false)->As<RE::MagicItem>()->IsHostile()) {
				
		// 		dodge::GetSingleton()->react_to_shouts_spells(actor, 2000.0f);
		// 	}
		// 	break;

		// case "PowerAttack_Start_end"_h:
		// case "NextAttackInitiate"_h:
		// 	dodge::GetSingleton()->react_to_melee_normal(actor, get_combat_reach(actor));
		// 	break;

		// case "NextPowerAttackInitiate"_h:

		// 	dodge::GetSingleton()->react_to_melee_power(actor, get_combat_reach(actor));
		// 	break;

		// case "bashPowerStart"_h:
		// 	dodge::GetSingleton()->react_to_bash(actor, 250.0f);
		// 	break;

		// case "BlockBashSprint"_h:
			
		// 	dodge::GetSingleton()->react_to_bash_sprint(actor, 400.0f); 
		// 	break;

		case "BowFullDrawn"_h:
			
			dodge::GetSingleton()->react_to_ranged(actor, 1500.0f);
			break;
		}
	}

	EventResult on_animation_event::ProcessEvent_NPC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		ProcessEvent(a_sink, a_event, a_eventSource);
		return _ProcessEvent_NPC(a_sink, a_event, a_eventSource);
	}

	EventResult on_animation_event::ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		ProcessEvent(a_sink, a_event, a_eventSource);
		return _ProcessEvent_PC(a_sink, a_event, a_eventSource);
	}

	ptr_CombatPath on_combatBehavior_backoff_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		// RE::Character* actor = a_actor->As<RE::Character>();
	
		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_back);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_back);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_circle_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		// RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_horizontal);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_horizontal);
			break;
		}
	
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_fallback_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		// RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_back);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_back);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_dodgethreat_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		// RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_all);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_all);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}
	


	inline void offset_NPC_rotation(RE::Actor* a_actor, float& a_angle) 
	{
		if (a_actor->IsPlayerRef()) {
			return;
		}
		float angleDelta = Utils::math::NormalRelativeAngle(a_angle - a_actor->data.angle.z);
	
		/* Dodge commitment */
		if (settings::bDodgeAI_Enable && dodge::GetSingleton()->get_is_dodging(a_actor)) {  
			angleDelta = 0;
		}
		a_angle = a_actor->data.angle.z + angleDelta;
	}

	
	void on_set_rotation::Actor_SetRotationX(RE::Actor* a_this, float a_angle)
	{
		offset_NPC_rotation(a_this, a_angle);
		_Actor_SetRotationX(a_this, a_angle);
	}

	void on_set_rotation::Actor_SetRotationZ(RE::Actor* a_this, float a_angle)
	{
		offset_NPC_rotation(a_this, a_angle);
		_Actor_SetRotationZ(a_this, a_angle);
	}

};
