#include "hooks.h"
#include "settings.h"
#include "perilous.h"
#include "dodge.h"
#include "AttackState.h"
#include "include/Utils.h"
#include "block.h"
namespace hooks
{
	//TODO: add stagger hook to negate bash stagger on perilous attackers.
	void install()
	{
		on_attack_action::install();
	}
	static inline RE::BSFixedString cPtr_bashPowerStart = "bashPowerStart";
	/// <summary>
	/// Handle perilous attacking.
	/// </summary>
	/// <param name="a_actionData"></param>
	/// <returns>Whether the attack action is performed.</returns>
	bool on_attack_action::perform_atk_action(RE::TESActionData* a_actionData)
	{
		if (!settings::bPerilousAttack_Enable || !a_actionData) {
			return _perform_atk_action(a_actionData);
		}
		//static RE::BSFixedString bashStart;
		//if (settings::bPerilousBash_Enable) { /* Turn all regular bash into power bash*/
		//	if (a_actionData->unk28 == "bashStart"sv) {
		//		a_actionData->unk28 = cPtr_bashPowerStart;
		//	}
		//}
		//return _perform_atk_action(a_actionData);
	}

	void on_animation_event::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		if (!a_event->holder) {
			return;
		}
		std::string_view eventTag = a_event->tag.data();
		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>();
		switch (hash(eventTag.data(), eventTag.size())) {
		case "SoundPlay.NPCHumanCombatShieldBashPower"_h:
			//if (settings::bPerilousBash_Enable) {
			//	perilous::GetSingleton()->attempt_start_perilous_attack(actor, perilous::PERILOUS_TYPE::blue);
			//}
			//break;
		case "preHitFrame"_h:
			if (AttackState::GetSingleton()->get_atk_state(actor->GetHandle()) != AttackState::atk_state::kMid) {  //none, end => start
				AttackState::GetSingleton()->set_atk_state(actor->GetHandle(), AttackState::AttackState::kStart);
			}
			if (settings::bPerilousAttack_Enable && Utils::Actor::isPowerAttacking(actor)) {
				perilous::GetSingleton()->attempt_start_perilous_attack(actor, perilous::PERILOUS_TYPE::red);
			}
			dodge::GetSingleton()->react_to_attack(actor);
			break;
		case "attackStop"_h:
		case "bashStop"_h:
			perilous::GetSingleton()->attempt_end_perilous_attack(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
			AttackState::GetSingleton()->set_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle(), AttackState::AttackState::kNone);
			break;
		// Start phase
		case "CastOKStart"_h:
		case "TDM_AttackStart"_h:
		case "Collision_AttackStart"_h:
		case "Collision_Start"_h:
			if (AttackState::GetSingleton()->get_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle()) != AttackState::AttackState::kMid) {
				AttackState::GetSingleton()->set_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle(), AttackState::AttackState::kStart);
			}
			break;
		// Mid phase
		case "weaponSwing"_h:
		case "weaponLeftSwing"_h:
		case "SoundPlay.WPNSwingUnarmed"_h:
		case "TDM_AttackMid"_h:
		case "Collision_Add"_h:
			if (AttackState::GetSingleton()->get_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle()) != AttackState::AttackState::kEnd) {
				AttackState::GetSingleton()->set_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle(), AttackState::AttackState::kMid);
			}
			break;
		// End phase
		case "HitFrame"_h: //attack actually happens
		case "attackWinStart"_h:
		case "SkySA_AttackWinStart"_h:
		case "MCO_WinOpen"_h:
		case "MCO_PowerWinOpen"_h:
		case "MCO_TransitionOpen"_h:
		case "MCO_Recovery"_h:
		case "TDM_AttackEnd"_h:
		case "Collision_AttackEnd"_h:
			AttackState::GetSingleton()->set_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle(), AttackState::AttackState::kEnd);
			break;
		// None phase
		case "TDM_AttackStop"_h:
		case "SkySA_AttackWinEnd"_h:
		case "MCO_WinClose"_h:
		case "MCO_PowerWinClose"_h:
		case "MCO_TransitionClose"_h:
			AttackState::GetSingleton()->set_atk_state(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>()->GetHandle(), AttackState::AttackState::kNone);
			break;
		//dodge phase setting
		case "MCO_DodgeInitiate"_h:
			dodge::GetSingleton()->set_dodge_phase(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>(), true);
			break;
		case "MCO_DodgeStop"_h:
		case "DodgeStop"_h:
			dodge::GetSingleton()->set_dodge_phase(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>(), false);
			break;
		case "blockStartOut"_h:
			Block::GetSingleton()->attempt_start_perfect_block(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
			break;
		case "blockStop"_h:
			Block::GetSingleton()->attempt_end_perfect_block(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
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
		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_all, true);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_all, true);
			break;
		}
		

		return _create_path(a_actor, a_newPos, a3, speed_ind);

	}

	static inline void stagger(RE::Actor* a_actor, RE::Actor* a_victim, float a_value = 1)
	{
		auto headingAngle = a_victim->GetHeadingAngle(a_actor);
		auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
		a_victim->SetGraphVariableFloat("staggerDirection", direction);
		a_victim->SetGraphVariableFloat("StaggerMagnitude", a_value);
		a_victim->NotifyAnimationGraph("staggerStart");
	}
	
	void on_melee_hit::processHit(RE::Actor* victim, RE::HitData& hitData)
	{
		RE::Actor* attacker = hitData.aggressor.get().get();
		if (attacker) {
			RE::ActorHandle handle;
			if (settings::bPerilousAttack_Enable && perilous::GetSingleton()->is_perilous_attacking(attacker, handle)) {
				stagger(attacker, victim);
			} else if (settings::bPerilousBash_Enable) {
				if (!victim->IsPlayerRef() && Utils::Actor::isBashing(attacker) && Utils::Actor::isBashing(victim)) {
					AnimSpeedManager::revertAnimSpeed(victim->GetHandle());
					stagger(attacker, victim, 10);
				}
			}
		}

		_ProcessHit(victim, hitData);
	}


	inline void offset_NPC_rotation(RE::Actor* a_actor, float& a_angle) 
	{
		if (a_actor->IsPlayerRef()) {
			return;
		}
		float angleDelta = Utils::math::NormalRelativeAngle(a_angle - a_actor->data.angle.z);
		/* Attack commitment */
		if (settings::bNPCCommitment_Enable && a_actor->IsAttacking()) { 
			switch (AttackState::GetSingleton()->get_atk_state(a_actor->GetHandle())) {
			case AttackState::atk_state::kStart:
				angleDelta *= settings::fNPCCommitment_AttackStartMult;
				break;
			case AttackState::atk_state::kMid:
				angleDelta *= settings::fNPCCommitment_AttackMidMult;
				break;
			case AttackState::atk_state::kEnd:
				angleDelta *= settings::fNPCCommitment_AttackEndMult;
				break;
			}
		}
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
