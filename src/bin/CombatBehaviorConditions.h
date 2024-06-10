#pragma once
#include "UselessFenixUtils.h"
#include "dodge.h"

namespace Movement
{
	using PA = Utils::PolarAngle;
	struct AttackInfo
	{
		float R;
		float attackAngle;
		float r;
		PA me;
		bool reflected;
	};
	const float default_angle = 50.0f;

	float get_FallbackDistance(RE::Character* me);
	// bool isInDanger(RE::Actor* me, AttackInfo* info = nullptr);

	enum class CircleDirestions
	{
		None,
		Left,
		Right,
		Back
	};

	namespace Dodging
	{
		uint32_t should(RE::Character*);
		uint32_t should_danger(RE::Character*);
	}

}

bool has_enoughStamina(RE::Actor* a);
void interruptattack(RE::Actor* me);
float get_combat_reach(RE::Actor* a);
float get_dist2(RE::Actor* a, RE::Actor* b);
static float get_angle_he_me(RE::Actor* me, RE::Actor* he, RE::BGSAttackData* attackdata);
bool is_powerattacking(RE::Actor* a);
