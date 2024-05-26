#include "UselessFenixUtils.h"
#include "CombatBehaviorConditions.h"
#include "dodge.h"



#define DODGE_ID 1
#define DODGE_SPEED 1.647f

using PA = Utils::PolarAngle;

bool has_manyStamina(RE::Actor* a)
{
	auto cur = a->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
	auto total = get_total_av(a, RE::ActorValue::kStamina);
	return total >= 100.0f && total * 0.5 <= cur;
}

bool has_enoughStamina(RE::Actor* a)
{
	auto cur = a->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
	auto total = get_total_av(a, RE::ActorValue::kStamina);
	return total >= 25.0f && total * 0.25 <= cur;
}

bool is_powerattacking(RE::Actor* a)
{
	return _generic_foo<37639, bool, RE::Actor*>(a);
}

bool is_staying(RE::Actor* a) {
	return !a->AsActorState()->actorState1.running && !a->AsActorState()->actorState1.walking && !a->AsActorState()->actorState1.sprinting;
}

bool has_stamina(RE::Actor* a)
{
	auto cur = a->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
	auto total = get_total_av(a, RE::ActorValue::kStamina);
	return total <= 20.0f || cur >= 15.0f;
}

bool is_staggered(RE::Actor* a)
{
	return _generic_foo<46064, bool, RE::Actor*>(a);
}

bool check_collisions(RE::Actor* a, RE::NiPoint3* a_pos, RE::NiPoint3* a_new_pos)
{
	return _generic_foo<46050, bool, RE::Actor*, RE::NiPoint3*, RE::NiPoint3*, float, float>(a, a_pos, a_new_pos, 2.0f, 64.0f);
}

bool is_blocking(RE::Actor* a)
{
	return _generic_foo<36927, bool, RE::Actor*>(a);
}

bool is_bashing(RE::Actor* a)
{
	if (a->AsActorState()->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kNone)
		return false;

	auto adata = Utils::get_attackData(a);
	if (!adata)
		return false;

	return adata->data.flags.any(RE::AttackData::AttackFlag::kBashAttack);
}

bool is_attacking(RE::Actor* a)
{
	auto adata = Utils::get_attackData(a);
	if (!adata)
		return false;
	auto state = a->AsActorState()->actorState1.meleeAttackState;
	return state == RE::ATTACK_STATE_ENUM::kDraw || state == RE::ATTACK_STATE_ENUM::kSwing || state == RE::ATTACK_STATE_ENUM::kHit;
}

bool is_juststarted_attacking(RE::Actor* a)
{
	auto adata = Utils::get_attackData(a);
	if (!adata)
		return false;
	auto state = a->AsActorState()->actorState1.meleeAttackState;
	return state == RE::ATTACK_STATE_ENUM::kDraw;
}

bool is_moving(RE::Actor* a)
{
	auto state = a->AsActorState()->actorState1;
	return state.running || state.walking || state.sprinting;
}

static float get_angle_he_me(RE::Actor* me, RE::Actor* he, RE::BGSAttackData* attackdata)
{
	auto he_me = PA(me->GetPosition() - he->GetPosition());
	auto head = PA(he->GetHeading(false) * 180.0f / PI);
	if (attackdata)
		head = head.add(attackdata->data.attackAngle);
	auto angle = he_me.sub(head).to_normangle();
	return angle;
}

void interruptattack(RE::Actor* me)
{
	me->NotifyAnimationGraph("attackStop");
	me->NotifyAnimationGraph("bashStop");
	me->NotifyAnimationGraph("blockStop");
	me->NotifyAnimationGraph("staggerStop");
	me->NotifyAnimationGraph("recoilStop");
}

bool is_AttackEnded(RE::Actor* a)
{
	auto state = a->AsActorState()->actorState1.meleeAttackState;
	return state != RE::ATTACK_STATE_ENUM::kDraw && state != RE::ATTACK_STATE_ENUM::kSwing && state != RE::ATTACK_STATE_ENUM::kNone;
}



float get_Reach(RE::Actor* a)
{
	return _generic_foo<37588, float, RE::Actor*>(a);
}

float get_dist2(RE::Actor* a, RE::Actor* b)
{
	return _generic_foo<46058, float, RE::Actor*, RE::Actor*>(a, b);
}

float get_combat_reach(RE::Actor* a)
{
	return get_Reach(a) + 30.0f;
}





namespace Movement
{
	using PA = Utils::PolarAngle;
	const float default_angle = 50.0f;

	enum class CircleDirestions
	{
		None,
		Left,
		Right,
		Back
	};

	struct AttackInfo
	{
		float R;
		float attackAngle;
		float r;
		PA me;
		bool reflected;
	};

	static float _get_circle_angle(float attackAngle, float angle, bool notreflected)
	{
		angle = fmaxf(attackAngle + angle + attackAngle * 0.2f, 20.0f);
		if (notreflected)
			angle = -angle;
		return angle;
	}

	void rotate(const RE::NiPoint3& me, const RE::NiPoint3& he, RE::NiPoint3& new_pos, float angle)
	{
		auto he_me = me - he;
		auto angle_sin = sin(angle);
		auto angle_cos = cos(angle);
		new_pos.x = he_me.x * angle_cos - he_me.y * angle_sin + he.x;
		new_pos.y = he_me.y * angle_cos + he_me.x * angle_sin + he.y;
		new_pos.z = he_me.z + he.z;
	}

	bool isInDanger(RE::Actor* me, AttackInfo* info = nullptr)
	{
		auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
		if (!he)
			return false;

		auto R = get_combat_reach(he);
		auto r2 = get_dist2(me, he);

		RE::BGSAttackData* attackdata = Utils::get_attackData(he);
		auto angle = get_angle_he_me(me, he, attackdata);

		float attackAngle = attackdata ? attackdata->data.strikeAngle : 50.0f;

		if (info) {
			info->R = R;
			info->r = sqrt(r2);
			info->reflected = angle < 0.0f;
			info->me = abs(angle);
			info->attackAngle = attackAngle;
		}

		if (is_blocking(he) || !is_attacking(he))
			return false;

		auto attackState = he->AsActorState()->GetAttackState();
		if (attackState != RE::ATTACK_STATE_ENUM::kSwing && attackState != RE::ATTACK_STATE_ENUM::kDraw) {
			return false;
		}
		if (abs(angle) > attackAngle) {
			return false;
		}

		if (r2 > R * R && (!is_powerattacking(he) || r2 > 500.0f * 500.0f)) {
			return false;
		}

		return true;
	}

	float get_FallbackDistance(const AttackInfo& info)
	{
		// info.r subs after
		return fmaxf(info.R - 137.76f, 60.0f);
	}

	float get_FallbackDistance(RE::Character* me)
	{
		AttackInfo info;
		if (!isInDanger(me, &info))
			return 80.0f;

		return get_FallbackDistance(info);
	}

	bool check_angle(RE::Actor* me, RE::Actor* he, const AttackInfo& info, float me_angle)
	{
		auto angle = _get_circle_angle(info.attackAngle, me_angle, !info.reflected) / 180.0f * PI;

		RE::NiPoint3 new_pos;
		rotate(me->GetPosition(), he->GetPosition(), new_pos, angle);

		bool ans = check_collisions(me, &me->data.location, &new_pos);

		return ans;
	}

	CircleDirestions choose_moving_direction_circle(const AttackInfo* const info, RE::Actor* a)
	{
		auto he = a->GetActorRuntimeData().currentCombatTarget.get().get();
		if (!he)
			return CircleDirestions::None;

		const float DIST_BORDER = 100.0f;

		const float r = info->r;
		const float me = info->me;

		if (PA::dist(r, info->attackAngle - me) <= DIST_BORDER && check_angle(a, he, *info, -me)) {
			return info->reflected ? CircleDirestions::Left : CircleDirestions::Right;
		} else if (PA::dist(r, info->attackAngle + me) <= DIST_BORDER && check_angle(a, he, *info, me)) {
			return info->reflected ? CircleDirestions::Right : CircleDirestions::Left;
		}

		return CircleDirestions::None;
	}

	namespace Dodging
	{

		template <bool change = false>
		uint32_t should_danger_alwaysDanger(RE::Character* me, RE::Actor*, const AttackInfo& info)
		{
			auto dir = choose_moving_direction_circle(&info, me);
			if (dir == CircleDirestions::Left || dir == CircleDirestions::Right) {
				if (change) {
					auto nir = dir == CircleDirestions::Left ? &dodge_directions_tk_right : &dodge_directions_tk_left;
					dodge::GetSingleton()->attempt_dodge(me, nir, true);
				}

				return true;
			}

			return false;
		}

		template <bool change = false>
		uint32_t should_alwaysDanger(RE::Character* a, RE::Actor* he, const AttackInfo& info)
		{
			const float DIST_BORDER = 60.0f;

			const float r = info.r;
			const float R = info.R;
			const PA me = info.me;
			float back_dist = R - r;

			if (!is_powerattacking(he) && back_dist <= DIST_BORDER) {
				RE::NiPoint3 he_me = a->GetPosition() - he->GetPosition(), new_pos;
				auto he_me_len = he_me.Unitize();
				auto walk_distance = get_FallbackDistance(info) - he_me_len;
				new_pos = he_me * walk_distance + a->GetPosition();

				if (check_collisions(a, &a->data.location, &new_pos)) {
					if (change) {
						dodge::GetSingleton()->attempt_dodge(a, &dodge_directions_tk_reactive, true);
					}
					return true;
				} else {
				}
			}

			return false;
		}

		uint32_t should([[maybe_unused]] RE::Character* me)
		{
			if (is_powerattacking(me) && !is_juststarted_attacking(me))
				return false;

			if (is_bashing(me))
				return false;

			AttackInfo info;
			if (!isInDanger(me, &info))
				return false;

			auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
			if (!he)
				return false;

			return should_alwaysDanger<true>(me, he, info);
		}

		uint32_t should_danger([[maybe_unused]] RE::Character* me)
		{
			if (is_powerattacking(me) && !is_juststarted_attacking(me))
				return false;

			if (is_bashing(me))
				return false;

			auto he = me->GetActorRuntimeData().currentCombatTarget.get().get();
			if (!he)
				return false;

			AttackInfo info;
			if (!isInDanger(me, &info)) 
				return false;

			return should_danger_alwaysDanger<true>(me, he, info);
		}

    }
}
