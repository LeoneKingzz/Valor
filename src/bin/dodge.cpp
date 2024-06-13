#include "dodge.h"

//#define PI 3.1415926535f
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;


void dodge::init() {
	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	if (_precision_API) {
		// precisionAPI->AddWeaponWeaponCollisionCallback(SKSE::GetPluginHandle(), OnMeleeHit::PrecisionWeaponsCallback);
		_precision_API->AddPreHitCallback(SKSE::GetPluginHandle(), DodgeCallback_PreHit);
		// precisionAPI->AddPostHitCallback(SKSE::GetPluginHandle(), OnMeleeHit::PrecisionWeaponsCallback_Post);
		logger::info("Enabled compatibility with Precision");
	}
	
}


//Native Functions for Papyrus
float dodge::GetProtaganist_ReflexScore(RE::Actor* a_actor){
	float Score = 0.0f;

 /////////////////////////////////////////////////Armour Weighting////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	auto Helm = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kHair);

	auto Chest = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kBody);

	auto Gauntlet = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kHands);

	auto Boots = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kFeet);

	auto Shield = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kShield);
	


	if (Helm) {
		switch (Helm->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Helm_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Helm_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Helm_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Helm_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Chest) {
		switch (Chest->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Chest_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Chest_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Chest_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Chest_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Gauntlet) {
		switch (Gauntlet->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Gauntlet_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Gauntlet_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Gauntlet_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Gauntlet_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Boots) {
		switch (Boots->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Boots_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Boots_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Boots_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Boots_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Utils::Actor::isEquippedShield(a_actor)) {
		switch (Shield->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Shield_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Shield_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Shield_weight * Protagnist_Reflexes.Armour_Weighting;
	}

 /////////////////////////////////////////////////Defensive & Skirmish Weighting ///////////////////////////////////////////////////////////////////////////////////////////

	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			Score += style->generalData.defensiveMult * Protagnist_Reflexes.Defensive_Weighting;

			Score += style->generalData.avoidThreatChance * CStyle.Skirmish_AvoidThreat_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.circleMult * CStyle.Skirmish_Circle_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.fallbackMult * CStyle.Skirmish_Fallback_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->longRangeData.strafeMult * CStyle.Skirmish_Strafe_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
		}
	}

 /////////////////////////////////////////////////Sneak Skill Weighting /////////////////////////////////////////////////////////////////////////////////////////////////////////

	Score += (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kSneak)/100.0f) * Protagnist_Reflexes.Sneak_Weighting;

	return Score;
}

bool dodge::BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->RegisterFunction("GetProtaganist_ReflexScore", "_SM_UND_NativeFunctions", GetProtaganist_ReflexScore);
	vm->RegisterFunction("Protagnist_can_dodge", "_SM_UND_NativeFunctions", Protagnist_can_dodge);
	return true;
}

PRECISION_API::PreHitCallbackReturn dodge::DodgeCallback_PreHit(const PRECISION_API::PrecisionHitData& a_precisionHitData)
{
	PRECISION_API::PreHitCallbackReturn returnData;
	if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
		return returnData;
	}

	auto actor = a_precisionHitData.target->As<RE::Actor>();

	if (actor->IsPlayerRef()) {
		return returnData;
	}

	if (!Utils::Actor::isHumanoid(actor)) {
		return returnData;
	}

	if (!ValhallaUtils::is_adversary(actor, a_precisionHitData.attacker)) {
		return returnData;
	}

	if (a_precisionHitData.attacker->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->GetWeaponType() == RE::WEAPON_TYPE::kBow || a_precisionHitData.attacker->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow) {
		return returnData;
	}

	bool bMaxsuWeaponParry_InWeaponParry = false;

	if ((actor)
			->GetGraphVariableBool("bMaxsuWeaponParry_InWeaponParry", bMaxsuWeaponParry_InWeaponParry) &&
		bMaxsuWeaponParry_InWeaponParry) {
		return returnData;
	}

	RE::BGSAttackData* attackdata = Utils::get_attackData(a_precisionHitData.attacker);
	auto angle = get_angle_he_me(actor, a_precisionHitData.attacker, attackdata);

	float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

	if (abs(angle) > attackAngle) {
		return returnData;
	}

	dodge::GetSingleton()->NormalAttack_attempt_dodge(actor, &dodge_directions_tk_reactive);

	return returnData;
}

/*Get the dodge chance of a reactive dodger in case of an incoming attack.*/
float dodge::get_dodge_chance(RE::Actor* a_actor) {
	float Score = 0.0f;

	/////////////////////////////////////////////////Armour Weighting////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	auto Helm = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kHair);

	auto Chest = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kBody);

	auto Gauntlet = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kHands);

	auto Boots = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kFeet);

	auto Shield = a_actor->GetWornArmor(RE::BIPED_OBJECTS::kShield);

	if (Helm) {
		switch (Helm->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Helm_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Helm_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Helm_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Helm_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Chest) {
		switch (Chest->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Chest_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Chest_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Chest_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Chest_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Gauntlet) {
		switch (Gauntlet->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Gauntlet_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Gauntlet_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Gauntlet_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Gauntlet_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Boots) {
		switch (Boots->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Boots_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Boots_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Boots_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Boots_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Utils::Actor::isEquippedShield(a_actor)) {
		switch (Shield->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Shield_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Shield_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Shield_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	/////////////////////////////////////////////////Defensive & Skirmish Weighting ///////////////////////////////////////////////////////////////////////////////////////////

	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			Score += style->generalData.defensiveMult * Protagnist_Reflexes.Defensive_Weighting;

			Score += style->generalData.avoidThreatChance * CStyle.Skirmish_AvoidThreat_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.circleMult * CStyle.Skirmish_Circle_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.fallbackMult * CStyle.Skirmish_Fallback_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->longRangeData.strafeMult * CStyle.Skirmish_Strafe_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
		}
	}

	/////////////////////////////////////////////////Sneak Skill Weighting /////////////////////////////////////////////////////////////////////////////////////////////////////////

	Score += (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kSneak) / 100.0f) * Protagnist_Reflexes.Sneak_Weighting;

	return Score;
}

float dodge::Get_ReactiveDodge_Distance(RE::Actor *actor) {
	
	auto defenderRightEquipped = actor->GetEquippedObject(false);
	auto distance = 250.0f;

	if (defenderRightEquipped && (defenderRightEquipped->IsWeapon())) {
		RE::TESObjectWEAP* weapon = (defenderRightEquipped->As<RE::TESObjectWEAP>());
		switch (weapon->GetWeaponType()) {
		case RE::WEAPON_TYPE::kOneHandSword:
			distance = 310.0f;
			break;
		case RE::WEAPON_TYPE::kOneHandAxe:
			distance = 305.0f;
			break;
		case RE::WEAPON_TYPE::kOneHandMace:
			distance = 300.0f;;
			break;
		case RE::WEAPON_TYPE::kOneHandDagger:
			distance = 250.0f;
			break;
		case RE::WEAPON_TYPE::kTwoHandAxe:
			distance = 350.0f;
			break;
		case RE::WEAPON_TYPE::kTwoHandSword:
			distance = 370.0;
			break;
		case RE::WEAPON_TYPE::kHandToHandMelee:
		    if (!Utils::Actor::isHumanoid(actor)) {
				distance = 350.0f;
			} else {
				distance = 130.0f;
			}
			break;
		}
	}
	return distance;
}

void dodge::Set_iFrames(RE::Actor* actor){
	actor->SetGraphVariableBool("bIframeActive", true);
	actor->SetGraphVariableBool("bInIframe", true);
}

void dodge::Reset_iFrames(RE::Actor* actor){
	actor->SetGraphVariableBool("bIframeActive", false);
	actor->SetGraphVariableBool("bInIframe", false);
}



/*Trigger reactive AI surrounding the attacker.*/
void dodge::react_to_melee(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR*_refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();
			
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;


			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// if (r2 > R * R && (!is_powerattacking(a_attacker) || r2 > 500.0f * 500.0f)) {
			// 	return RE::BSContainer::ForEachResult::kContinue;
			// }

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_melee_power(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* _refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();

			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// if (r2 > R * R && (!is_powerattacking(a_attacker) || r2 > 500.0f * 500.0f)) {
			// 	return RE::BSContainer::ForEachResult::kContinue;
			// }

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->Powerattack_attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_melee_normal(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* _refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();

			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// if (r2 > R * R && (!is_powerattacking(a_attacker) || r2 > 500.0f * 500.0f)) {
			// 	return RE::BSContainer::ForEachResult::kContinue;
			// }

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->NormalAttack_attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_bash(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR*_refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();
			
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			/*RE::Character* a_refr = refr->As<RE::Character>();*/
			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->Bash_attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_bash_sprint(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* _refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();

			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			/*RE::Character* a_refr = refr->As<RE::Character>();*/
			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->BashSprint_attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_ranged(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR*_refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();
			
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			bool hasLOS = false;
			if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 7.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_shouts_spells(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* _refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();

			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			bool hasLOS = false;
			if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->Shouts_Spells_attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_shouts_spells_fast(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* _refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();

			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			bool hasLOS = false;
			if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			// auto R = attack_range;
			// auto r2 = get_dist2(refr, a_attacker);

			RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
			auto angle = get_angle_he_me(refr, a_attacker, attackdata);

			float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

			if (abs(angle) > attackAngle) {
				return RE::BSContainer::ForEachResult::kContinue;
			}

			switch (settings::iDodgeAI_Framework) {
			case 0:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_all);
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::set_dodge_phase(RE::Actor* a_dodger, bool a_isDodging)
{
	auto handle = a_dodger->GetHandle();
	if (handle) {
		writeLock l (dodging_actors_lock);
		if (a_isDodging) {
			if (!dodging_actors.contains(handle)) {
				dodging_actors.insert(handle);
			}
		} else {
			dodging_actors.erase(handle);
		}
	}
}

bool dodge::get_is_dodging(RE::Actor* a_actor)
{
	auto handle = a_actor->GetHandle();
	if (handle) {
		readLock l(dodging_actors_lock);
		return dodging_actors.contains(handle);
	}
	return false;
	
}


/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	auto attackState = a_actor->AsActorState()->GetAttackState();
	// auto IsStaggered = static_cast<bool>(a_actor->AsActorState()->actorState2.staggered);
	auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
	// auto IsStaggeredCT = static_cast<bool>(CombatTarget->AsActorState()->actorState2.staggered);
	// auto RecoilState = static_cast<int>(a_actor->AsActorState()->actorState2.recoil);
	// auto CT_RecoilState = static_cast<int>(CombatTarget->AsActorState()->actorState2.recoil);


	if (settings::bZUPA_mod_Check) {
		const auto magicEffect = RE::TESForm::LookupByEditorID("zxlice_cooldownEffect")->As<RE::EffectSetting>();
		auto magicTarget = a_actor->AsMagicTarget();
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough) && !magicTarget->HasMagicEffect(magicEffect)) {
			return true;
		}
	} else if (settings::bUAPNG_mod_Check){
		bool IUBusy = false;
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& (a_actor->GetGraphVariableBool("IUBusy", IUBusy) && !IUBusy) && a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}

	} else{
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}
	}
	return false;
}

/*Attempt to dodge an incoming threat, choosing one of the directions from A_DIRECTIONS.*/
void dodge::attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }
	if (get_is_dodging(a_actor)) {
		return;
	}

	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Powerattack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }
	if (get_is_dodging(a_actor)) {
		return;
	}

	// auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

	// auto CW = CombatTarget->GetActorRuntimeData().currentProcess->GetEquippedRightHand()->As<RE::TESObjectWEAP>()->GetWeaponType();

	// if (CW) {
	// 	if (CW == RE::WEAPON_TYPE::kTwoHandSword || RE::WEAPON_TYPE::kTwoHandAxe){
	// 		Sleep(dodge::GetSingleton()->GenerateRandomInt(300, 500));

	// 	} else {
	// 		Sleep(dodge::GetSingleton()->GenerateRandomInt(200, 400));
	// 	}
	// } else {
	// 	Sleep(dodge::GetSingleton()->GenerateRandomInt(200, 400));
	// }

	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::NormalAttack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }

	if (get_is_dodging(a_actor)) {
		return;
	}

	// auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

	// auto CW = CombatTarget->GetActorRuntimeData().currentProcess->GetEquippedRightHand()->As<RE::TESObjectWEAP>()->GetWeaponType();

	// if (CW) {
	// 	if (CW == RE::WEAPON_TYPE::kTwoHandSword || RE::WEAPON_TYPE::kTwoHandAxe) {
	// 		Sleep(dodge::GetSingleton()->GenerateRandomInt(50, 200));

	// 	} else {
	// 		Sleep(dodge::GetSingleton()->GenerateRandomInt(0, 150));
	// 	}
	// } else {
	// 	Sleep(dodge::GetSingleton()->GenerateRandomInt(0, 150));
	// }

	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Shouts_Spells_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }

	if (get_is_dodging(a_actor)) {
		return;
	}

	// Sleep(dodge::GetSingleton()->GenerateRandomInt(200, 400));

	
	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Bash_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }

	if (get_is_dodging(a_actor)) {
		return;
	}

	// Sleep(dodge::GetSingleton()->GenerateRandomInt(100, 200));

	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::BashSprint_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	// if (ValhallaUtils::isBackFacing(CombatTarget, a_actor)) {  //no need to react to an attack if the attacker isn't facing you.
	// 	return;
	// }

	if (get_is_dodging(a_actor)) {
		return;
	}

	// Sleep(dodge::GetSingleton()->GenerateRandomInt(500, 900));

	float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor);

	logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);

	std::mt19937 gen(rd());
	// /*Check dodge chance using PRNG*/
	// std::uniform_real_distribution<> dis(0.f, 1.f);
	// if (dis(gen) > dodge_chance) {

	// 	return;
	// }
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				set_dodge_phase(a_actor, true);
				do_dodge(a_actor, direction);
			}
			return;
		} else {
			return;
		}
	}
}

///*Check if the actor is able to dodge.*/
//bool dodge::able_dodge(RE::Actor* a_actor)
//{
//	auto attackState = a_actor->AsActorState()->GetAttackState();
//	// auto IsStaggered = static_cast<bool>(a_actor->AsActorState()->actorState2.staggered);
//	auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
//	// auto IsStaggeredCT = static_cast<bool>(CombatTarget->AsActorState()->actorState2.staggered);
//	// auto RecoilState = static_cast<int>(a_actor->AsActorState()->actorState2.recoil);
//	// auto CT_RecoilState = static_cast<int>(CombatTarget->AsActorState()->actorState2.recoil);
//
//
//	if (settings::bZUPA_mod_Check) {
//		const auto magicEffect = RE::TESForm::LookupByEditorID("zxlice_cooldownEffect")->As<RE::EffectSetting>();
//		auto magicTarget = a_actor->AsMagicTarget();
//		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
//		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
//		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
//		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough) && !magicTarget->HasMagicEffect(magicEffect)) {
//			return true;
//		}
//	} else if (settings::bUAPNG_mod_Check){
//		bool IUBusy = false;
//		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
//		&& (a_actor->GetGraphVariableBool("IUBusy", IUBusy) && !IUBusy) && a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
//		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
//		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
//			return true;
//		}
//
//	} else{
//		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
//		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
//		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
//		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
//			return true;
//		}
//	}
//	return false;
//}



bool dodge::Protagnist_can_dodge(RE::Actor* a_actor)
{
	auto attackState = a_actor->AsActorState()->GetAttackState();
	// auto IsStaggered = static_cast<bool>(a_actor->AsActorState()->actorState2.staggered);
	auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
	// auto IsStaggeredCT = static_cast<bool>(CombatTarget->AsActorState()->actorState2.staggered);
	// auto RecoilState = static_cast<int>(a_actor->AsActorState()->actorState2.recoil);
	// auto CT_RecoilState = static_cast<int>(CombatTarget->AsActorState()->actorState2.recoil);


	if (settings::bZUPA_mod_Check) {
		const auto magicEffect = RE::TESForm::LookupByEditorID("zxlice_cooldownEffect")->As<RE::EffectSetting>();
		auto magicTarget = a_actor->AsMagicTarget();
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough) && !magicTarget->HasMagicEffect(magicEffect)) {
			return true;
		}
	} else if (settings::bUAPNG_mod_Check){
		bool IUBusy = false;
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& (a_actor->GetGraphVariableBool("IUBusy", IUBusy) && !IUBusy) && a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}

	} else{
		if (!a_actor->IsInKillMove() && !CombatTarget->IsInKillMove() && !CombatTarget->AsActorState()->IsBleedingOut()
		&& a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fSideStep_staminacost 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}
	}
	return false;
}


#define MAX_DIST_DIFFERENCE 50
bool dodge::can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest)
{
	bool canNavigate = false;
	bool noObstacle = true;
	RE::NiPoint3 raycast_dest = a_dest;
	RE::NiPoint3 nav_dest = a_dest;

	bool gotoNavdest = false;
	/*float expect_dist = a_actor->GetPosition().GetDistance(a_dest);*/
	
	// /*Use Skyrim's internal check*/
	// if (a_actor->UpdateNavPos(a_actor->GetPosition(), nav_dest, 4.0f, a_actor->GetBoundRadius()) 
	// 	&& abs(nav_dest.GetDistance(a_actor->GetPosition()) - expect_dist) < MAX_DIST_DIFFERENCE) {
	// 	RE::NiPoint3 nav_dest_raycast = nav_dest;                   // make a copy of nav_dest to use for raycast
	// 	if (DtryUtils::rayCast::object_exists(nav_dest_raycast)) {//check if the actor can stand on the nav dest
	// 		gotoNavdest = true;
	// 		canNavigate = true;
	// 	}
	// }
	
	/*Use our own pathing check, if skyrim's check fails.*/
	if (!gotoNavdest) {
		canNavigate = DtryUtils::rayCast::object_exists(raycast_dest, 75.f);//bigger range to account for slopes
	}
	
	if (canNavigate) {
		RE::NiPoint3 dest = gotoNavdest ? nav_dest : raycast_dest;
		//if (settings::bDodgeAI_DebugDraw_Enable) {
		//	if (API::TrueHUD_API_acquired) {
		//		API::_TrueHud_API->DrawLine(a_actor->GetPosition(), dest, 1.f, 0xff00ff);  //green line
		//	}
		//}
		
		/*Cast 3 rays from the actor, parallel to the dodging path to check for any obstacles.*/
		float obstacleDist = 0; /*Distance to the obstacle, if any*/
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.25f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.5f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.75f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
	}
	

	return canNavigate && noObstacle;
	
}


/*Get the direction the actor should dodge in.*/
//dodge_direction dodge::get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker)
//{
//	return dodge_direction::kForward; /*defaults to backward dodging for now*/
//}

int dodge::GenerateRandomInt(int value_a, int value_b) {

	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> dist(value_a, value_b);
	return dist(generator);
}

float dodge::GenerateRandomFloat(float value_a, float value_b)
{
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> dist(value_a, value_b);
	return dist(generator);
}

static const char* GVI_dodge_dir = "Dodge_Direction";
// static const char* AE_dodge = "Dodge";
void dmco_dodge(RE::Actor* a_actor, dodge_direction a_direction, const char* a_event) {
	auto task = SKSE::GetTaskInterface();
	if (!task) {
		return;
	}
	task->AddTask([a_actor, a_direction, a_event]() {
		a_actor->SetGraphVariableInt(GVI_dodge_dir, a_direction);
		interruptattack(a_actor);
		a_actor->NotifyAnimationGraph(a_event);
	});
}

void dodge::TRKE_dodge(RE::Actor* actor, const char* a_event, bool backingoff)
{
	if (backingoff) {
		actor->SetGraphVariableInt("iStep", 2);
		actor->NotifyAnimationGraph(a_event);
		return;
	}

	if (settings::bHasSilentRollperk_enable == 1) {
		auto bSilentRoll = actor->HasPerk(RE::BGSPerk::LookupByEditorID("SilentRoll")->As<RE::BGSPerk>());
		if (dodge::GetSingleton()->GenerateRandomInt(0, 10) <= settings::iDodgeRoll_ActorScaled_Chance && bSilentRoll && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fDodgeRoll_staminacost) {
			actor->SetGraphVariableInt("iStep", 0);
		} else {
			actor->SetGraphVariableInt("iStep", 2);
		}
		actor->NotifyAnimationGraph(a_event);
		return;

	} else {
		if (dodge::GetSingleton()->GenerateRandomInt(0, 10) <= settings::iDodgeRoll_ActorScaled_Chance && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= settings::fDodgeRoll_staminacost) {
			actor->SetGraphVariableInt("iStep", 0);	
		} else {
			actor->SetGraphVariableInt("iStep", 2);
		}
		actor->NotifyAnimationGraph(a_event);

	}
}

void dodge::do_dodge(RE::Actor* a_actor, dodge_direction a_direction)
{
	switch (a_direction) {
	case dodge_direction::kForward:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeForward");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kBackward:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeBack");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kLeft:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeLeft");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRight:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeRight");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	/*Only possible for DMCO*/
	case dodge_direction::kLeftBackward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kLeftForward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRightBackward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRightForward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	}
}
/*Get relative dodge vector*/
RE::NiPoint3 dodge::get_dodge_vector(dodge_direction a_direction)
{
	RE::NiPoint3 ret;
	ret.z = 0;
	switch (a_direction) {
	case kForward:
		ret.x = 0;
		ret.y = settings::fDodgeAI_DodgeDist;
		break;
	case kBackward:
		ret.x = 0;
		ret.y = -settings::fDodgeAI_DodgeDist;
		break;
	case kLeft:
		ret.x = -settings::fDodgeAI_DodgeDist;
		ret.y = 0;
		break;
	case kRight:
		ret.x = settings::fDodgeAI_DodgeDist;
		ret.y = 0;
		break;
	case kLeftBackward:
		ret.x = -settings::fDodgeAI_DodgeDist2;
		ret.y = -settings::fDodgeAI_DodgeDist2;
		break;
	case kLeftForward:
		ret.x = -settings::fDodgeAI_DodgeDist2;
		ret.y = settings::fDodgeAI_DodgeDist2;
		break;
	case kRightBackward:
		ret.x = settings::fDodgeAI_DodgeDist2;
		ret.y = -settings::fDodgeAI_DodgeDist2;
		break;
	case kRightForward:
		ret.x = settings::fDodgeAI_DodgeDist2;
		ret.y = settings::fDodgeAI_DodgeDist2;
		break;
	}
	
	return ret;
}

// namespace Movement
// {

// 	static float _get_circle_angle(float attackAngle, float angle, bool notreflected)
// 	{
// 		angle = fmaxf(attackAngle + angle + attackAngle * 0.2f, 20.0f);
// 		if (notreflected)
// 			angle = -angle;
// 		return angle;
// 	}

// 	void rotate(const RE::NiPoint3& me, const RE::NiPoint3& he, RE::NiPoint3& new_pos, float angle)
// 	{
// 		auto he_me = me - he;
// 		auto angle_sin = sin(angle);
// 		auto angle_cos = cos(angle);
// 		new_pos.x = he_me.x * angle_cos - he_me.y * angle_sin + he.x;
// 		new_pos.y = he_me.y * angle_cos + he_me.x * angle_sin + he.y;
// 		new_pos.z = he_me.z + he.z;
// 	}

// 	bool isInDanger(RE::Actor* me, AttackInfo* info = nullptr)
// 	{
// 		auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
// 		if (!he)
// 			return false;

// 		auto R = get_combat_reach(he);
// 		auto r2 = get_dist2(me, he);

// 		RE::BGSAttackData* attackdata = Utils::get_attackData(he);
// 		auto angle = get_angle_he_me(me, he, attackdata);

// 		float attackAngle = attackdata ? attackdata->data.strikeAngle : 50.0f;

// 		if (info) {
// 			info->R = R;
// 			info->r = sqrt(r2);
// 			info->reflected = angle < 0.0f;
// 			info->me = abs(angle);
// 			info->attackAngle = attackAngle;
// 		}

// 		if (is_blocking(he) || !is_attacking(he))
// 			return false
// 		;

// 		auto attackState = he->AsActorState()->GetAttackState();
// 		if (attackState != RE::ATTACK_STATE_ENUM::kSwing && attackState != RE::ATTACK_STATE_ENUM::kDraw) {
// 			return false;
// 		}
// 		if (abs(angle) > attackAngle) {
// 			return false;
// 		}

// 		if (r2 > R * R && (!is_powerattacking(he) || r2 > 500.0f * 500.0f)) {
// 			return false;
// 		}

// 		return true;
// 	}

// 	float get_FallbackDistance(const AttackInfo& info)
// 	{
// 		// info.r subs after
// 		return fmaxf(info.R - 137.76f, 60.0f);
// 	}

// 	float get_FallbackDistance(RE::Character* me)
// 	{
// 		AttackInfo info;
// 		if (!isInDanger(me, &info))
// 			return 80.0f;

// 		return get_FallbackDistance(info);
// 	}

// 	bool check_angle(RE::Actor* me, RE::Actor* he, const AttackInfo& info, float me_angle)
// 	{
// 		auto angle = _get_circle_angle(info.attackAngle, me_angle, !info.reflected) / 180.0f * PI;

// 		RE::NiPoint3 new_pos;
// 		rotate(me->GetPosition(), he->GetPosition(), new_pos, angle);

// 		bool ans = check_collisions(me, &me->data.location, &new_pos);

// 		return ans;
// 	}

// 	CircleDirestions choose_moving_direction_circle(const AttackInfo* const info, RE::Actor* a)
// 	{
// 		auto he = a->GetActorRuntimeData().currentCombatTarget.get().get();
// 		if (!he)
// 			return CircleDirestions::None;

// 		const float DIST_BORDER = 100.0f;

// 		const float r = info->r;
// 		const float me = info->me;

// 		if (PA::dist(r, info->attackAngle - me) <= DIST_BORDER && check_angle(a, he, *info, -me)) {
// 			return info->reflected ? CircleDirestions::Left : CircleDirestions::Right;
// 		} else if (PA::dist(r, info->attackAngle + me) <= DIST_BORDER && check_angle(a, he, *info, me)) {
// 			return info->reflected ? CircleDirestions::Right : CircleDirestions::Left;
// 		}

// 		return CircleDirestions::None;
// 	}

// 	namespace Dodging
// 	{

// 		template <bool change = false>
// 		uint32_t should_danger_alwaysDanger(RE::Character* me, RE::Actor*, const AttackInfo& info)
// 		{
// 			auto dir = choose_moving_direction_circle(&info, me);
// 			if (dir == CircleDirestions::Left || dir == CircleDirestions::Right) {
// 				if (change) {
// 					auto nir = dir == CircleDirestions::Left ? &dodge_directions_tk_right : &dodge_directions_tk_left;
// 					dodge::GetSingleton()->attempt_dodge(me, nir, true);
// 				}

// 				return true;
// 			}

// 			return false;
// 		}

// 		template <bool change = false>
// 		uint32_t should_alwaysDanger(RE::Character* a, RE::Actor* he, const AttackInfo& info)
// 		{
// 			const float DIST_BORDER = 60.0f;

// 			const float r = info.r;
// 			const float R = info.R;
// 			const PA me = info.me;
// 			float back_dist = R - r;

// 			if (!is_powerattacking(he) && back_dist <= DIST_BORDER) {
// 				RE::NiPoint3 he_me = a->GetPosition() - he->GetPosition(), new_pos;
// 				auto he_me_len = he_me.Unitize();
// 				auto walk_distance = get_FallbackDistance(info) - he_me_len;
// 				new_pos = he_me * walk_distance + a->GetPosition();

// 				return false;

// 				if (check_collisions(a, &a->data.location, &new_pos)) {
// 					if (change) {
// 						dodge::GetSingleton()->attempt_dodge(a, &dodge_directions_tk_reactive, true);
// 					}
// 					return true;
// 				} else {
// 				}
// 			}

// 			return false;
// 		}

// 		uint32_t should([[maybe_unused]] RE::Character* me)
// 		{
// 			/*if (is_powerattacking(me) && !is_juststarted_attacking(me))
// 				return false;

// 			if (is_bashing(me))
// 				return false;*/

// 			AttackInfo info;
// 			if (!isInDanger(me, &info))
// 				return false;

// 			auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
// 			if (!he)
// 				return false;

// 			return should_alwaysDanger<true>(me, he, info);
// 		}

// 		uint32_t should_danger([[maybe_unused]] RE::Character* me)
// 		{
// 			/*if (is_powerattacking(me) && !is_juststarted_attacking(me))
// 				return false;*/

// 			/*if (is_bashing(me))
// 				return false;*/

// 			auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
// 			if (!he)
// 				return false;

// 			AttackInfo info;
// 			if (!isInDanger(me, &info))
// 				return false;

// 			return should_danger_alwaysDanger<true>(me, he, info);
// 		}

// 	}
// }
