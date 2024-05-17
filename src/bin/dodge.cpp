#include "dodge.h"
#include "settings.h"
#include "include/Utils.h"
#include <algorithm>
#include "RE/M/Misc.h"
#include "CombatBehaviorConditions.h"
#define PI 3.1415926535f
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;

/*Get the dodge chance of a reactive dodger in case of an incoming attack.*/
float get_dodge_chance(RE::Actor* a_dodger) {
	if (a_dodger->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_dodger->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			return style->generalData.avoidThreatChance * settings::fDodgeAI_Chance_Mult;
		}
	}
	return 0.f;
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



/*Trigger reactive AI surrounding the attacker.*/
void dodge::react_to_melee(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR*_refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();
			
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || refr->IsInKillMove() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (ValhallaUtils::isBackFacing(a_attacker, refr)) {  //no need to react to an attack if the attacker isn't facing you.
				return RE::BSContainer::ForEachResult::kContinue;
			}
			RE::Character* a_refr = refr->As<RE::Character>();
			switch (settings::iDodgeAI_Framework) {
			case 0:
				Movement::Dodging::should(a_refr);
				break;
			case 1:
				Movement::Dodging::should(a_refr);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

void dodge::react_to_ranged_and_shouts(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR*_refr) {
		if (!_refr->IsDisabled() && _refr->GetFormType() == RE::FormType::ActorCharacter && _refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
			RE::Actor* refr = _refr->As<RE::Actor>();
			
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || refr->IsInKillMove() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			bool hasLOS = false;
			if (!(refr->HasLineOfSight(a_attacker, hasLOS) && hasLOS)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (ValhallaUtils::isBackFacing(a_attacker, refr)) {  //no need to react to an attack if the attacker isn't facing you.
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

/*Attempt to dodge an incoming threat, choosing one of the directions from A_DIRECTIONS.*/
void dodge::attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	if (!settings::bDodgeAI_Enable) {
		return;
	}
	
	float dodge_chance = a_forceDodge ? 1.f : get_dodge_chance(a_actor);
	
	std::mt19937 gen(rd());
	/*Check dodge chance using PRNG*/
	std::uniform_real_distribution<> dis(0.f, 1.f);
	if (dis(gen) > dodge_chance) {
		
		return;
	}
	
	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen); 


	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor) == true) {
			do_dodge(a_actor, direction);
			
			return;
		} else {
			
			return;
		}
	}
}



/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	const auto magicEffect = RE::TESForm::LookupByEditorID("zxlice_cooldownEffect")->As<RE::EffectSetting>();
	auto magicTarget = a_actor->AsMagicTarget();
	
	if (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= 25 && !magicTarget->HasMagicEffect(magicEffect)){
		return true;
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

void dodge::do_dodge(RE::Actor* a_actor, dodge_direction a_direction)
{
	switch (a_direction) {
	case dodge_direction::kForward:
		switch (settings::iDodgeAI_Framework) {
		case 0: 
			a_actor->NotifyAnimationGraph("TKDodgeForward");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kBackward:
		switch (settings::iDodgeAI_Framework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeBack");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kLeft:
		switch (settings::iDodgeAI_Framework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeLeft");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRight:
		switch (settings::iDodgeAI_Framework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeRight");
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
