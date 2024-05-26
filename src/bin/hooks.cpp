#include "hooks.h"
#include "settings.h"
#include "dodge.h"
#include "include/Utils.h"
#include "CombatBehaviorConditions.h"

namespace hooks
{

	static inline RE::BSFixedString cPtr_bashPowerStart = "bashPowerStart";
	/// <summary>
	/// Handle perilous attacking.
	/// </summary>
	/// <param name="a_actionData"></param>
	/// <returns>Whether the attack action is performed.</returns>
	// "MCO_DodgeInitiate"_h:

	void on_animation_event::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		if (!a_event->holder) {
			return;
		}
		std::string_view eventTag = a_event->tag.data();
		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>();
		switch (hash(eventTag.data(), eventTag.size())) {
		case "blockStop"_h:
		case "FootLeft"_h:
		case "FootRight"_h:
		case "staggerStop"_h:
		case "recoilStop"_h:
		case "blockHitStart"_h:
		case "blockHitStop"_h:
		case "bashStop"_h:
		case "blockStart"_h:
		case "attackStop"_h:
		case "MCO_Recovery"_h:
		    if (Utils::Actor::isHumanoid(actor) && !actor->IsPlayerRef() && actor->IsInCombat()) {
				RE::Character* a_actor = actor->As<RE::Character>();
				Movement::Dodging::should(a_actor);
			}
			break;

		case "bashPowerStart"_h:
		case "BlockBashSprint"_h:
		case "SoundPlay.NPCHumanCombatShieldBashPower"_h:
		    dodge::GetSingleton()->react_to_bash(actor, 350.0f);
			break;

		case "PowerAttack_Start_end"_h:
		case "NextAttackInitiate"_h:
		case "NextPowerAttackInitiate"_h:
		case "preHitFrame"_h:
		    dodge::GetSingleton()->react_to_melee(actor, 500.0f);
			break;

		case "BowFullDrawn"_h:
		    if ((dodge::GetSingleton()->GenerateRandomInt(0, 10)) <= 1)	{
				dodge::GetSingleton()->react_to_ranged_and_shouts(actor, 1500.0f);
			}
			break;
			
		case "Voice_SpellFire_Event"_h:
		case "MLh_SpellFire_Event"_h:
		case "MRh_SpellFire_Event"_h:
			dodge::GetSingleton()->react_to_ranged_and_shouts(actor, 1500.0f);
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
		RE::Character* actor = a_actor->As<RE::Character>();
	
		switch (settings::iDodgeAI_Framework) {
		case 0:
			// Movement::Dodging::should(actor); 
			break;
		case 1:
			// Movement::Dodging::should(actor);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_circle_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			// Movement::Dodging::should_danger(actor);
			break;
		case 1:
			// Movement::Dodging::should_danger(actor);
			break;
		}
	
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_fallback_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			// Movement::Dodging::should(actor);
			break;
		case 1:
			// Movement::Dodging::should(actor);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_dodgethreat_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		RE::Character* actor = a_actor->As<RE::Character>();

		switch (settings::iDodgeAI_Framework) {
		case 0:
			// Movement::Dodging::should_danger(actor);
			break;
		case 1:
			// Movement::Dodging::should_danger(actor);
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
