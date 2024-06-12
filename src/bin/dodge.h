#pragma once
#include "PCH.h"
#include "CombatBehaviorConditions.h"
#include <unordered_set>
#include <shared_mutex>
#include "UselessFenixUtils.h"
#include "settings.h"
#include "include/Utils.h"
#include <algorithm>
#include "RE/M/Misc.h"
#include "include/lib/PrecisionAPI.h"


using PRECISION_API::PreHitCallback;
using std::string;

enum dodge_direction
{
	kForward = 1,
	kRightForward = 2,
	kRight = 3,
	kRightBackward = 4,
	kBackward = 5,
	kLeftBackward = 6,
	kLeft = 7,
	kLeftForward = 8
};
typedef std::vector<dodge_direction> dodge_dir_set;

const dodge_dir_set dodge_directions_tk_horizontal = { dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_back = { dodge_direction::kBackward };
const dodge_dir_set dodge_directions_tk_vertical = { dodge_direction::kForward, dodge_direction::kBackward };
const dodge_dir_set dodge_directions_tk_all = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_reactive = { dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_right = { dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_left = { dodge_direction::kLeft };

const dodge_dir_set dodge_directions_dmco_horizontal = { dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_left = { dodge_direction::kLeft, dodge_direction::kLeftForward, dodge_direction::kLeftBackward };
const dodge_dir_set dodge_directions_dmco_right = { dodge_direction::kRight, dodge_direction::kRightForward, dodge_direction::kRightBackward };

const dodge_dir_set dodge_directions_dmco_back = { dodge_direction::kBackward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_vertical = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_reactive = { dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kBackward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_all = {
	dodge_direction::kForward,
	dodge_direction::kBackward,
	dodge_direction::kLeft,
	dodge_direction::kRight,
	dodge_direction::kLeftForward,
	dodge_direction::kRightForward,
	dodge_direction::kLeftBackward,
	dodge_direction::kRightBackward
};

class dodge
{
private:
	std::random_device rd;

public:

	void init();

	static dodge* GetSingleton()
	{
		static dodge singleton;
		return &singleton;
	}

	void react_to_melee(RE::Actor* a_attacker, float attack_range);
	void react_to_melee_power(RE::Actor* a_attacker, float attack_range);
	void react_to_melee_normal(RE::Actor* a_attacker, float attack_range);
	void react_to_bash(RE::Actor* a_attacker, float attack_range);
	void react_to_bash_sprint(RE::Actor* a_attacker, float attack_range);
	void react_to_ranged(RE::Actor* a_attacker, float attack_range);
	void react_to_shouts_spells(RE::Actor* a_attacker, float attack_range);
	void react_to_shouts_spells_fast(RE::Actor* a_attacker, float attack_range);

	struct
	{
		inline static float Heavyarm_mult = 0.25f;
		inline static float Lightarm_mult = 0.75f;
		inline static float clothing_mult = 1.0f;

		inline static float Helm_weight = 0.1f;
		inline static float Gauntlet_weight = 0.1f;
		inline static float Boots_weight = 0.1f;
		inline static float Chest_weight = 0.6f;
		inline static float Shield_weight = 0.1f;

	} inline static Armour;

	struct
	{
		inline static float Sneak_Weighting = 0.2f;
		inline static float Skirmish_Weighting = 0.2f;
		inline static float Defensive_Weighting = 0.3f;
		inline static float Armour_Weighting = 0.3f;

	} inline static Protagnist_Reflexes;

	struct
	{
		inline static float Skirmish_AvoidThreat_Weighting = 0.25f;
		inline static float Skirmish_Fallback_Weighting = 0.3f;
		inline static float Skirmish_Circle_Weighting = 0.3f;
		inline static float Skirmish_Strafe_Weighting = 0.15f;

	} inline static CStyle;

	static float GetProtaganist_ReflexScore(RE::Actor* a_actor);

	static bool Protagnist_can_dodge(RE::Actor* a_actor);

	static bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);

	int GenerateRandomInt(int value_a, int value_b);
	float GenerateRandomFloat(float value_a, float value_b);
		
	void attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Powerattack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void NormalAttack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Shouts_Spells_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Bash_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void BashSprint_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);

	static void Set_iFrames(RE::Actor* actor);
	static void Reset_iFrames(RE::Actor* actor);

	void set_dodge_phase(RE::Actor* a_dodger, bool a_isDodging);
	bool get_is_dodging(RE::Actor* a_actor);

	float Get_ReactiveDodge_Distance(RE::Actor *actor);
	float get_dodge_chance(RE::Actor* a_actor);

private:

	PRECISION_API::IVPrecision4* _precision_API;

	static PRECISION_API::PreHitCallbackReturn DodgeCallback_PreHit(const PRECISION_API::PrecisionHitData& a_precisionHitData);

	bool able_dodge(RE::Actor* a_actor);

	bool can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest);
	
	/*dodge_direction get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker);*/

	void do_dodge(RE::Actor* a_actor, dodge_direction a_direction);
	void TRKE_dodge(RE::Actor* actor, const char* a_event, bool backingoff = false);

	RE::NiPoint3 get_dodge_vector(dodge_direction a_direction);

	std::unordered_set<RE::ActorHandle> dodging_actors;
	mutable std::shared_mutex dodging_actors_lock;
	
};

// namespace Movement
// {
// 	using PA = Utils::PolarAngle;
// 	struct AttackInfo
// 	{
// 		float R;
// 		float attackAngle;
// 		float r;
// 		PA me;
// 		bool reflected;
// 	};
// 	const float default_angle = 50.0f;

// 	float get_FallbackDistance(RE::Character* me);
// 	// bool isInDanger(RE::Actor* me, AttackInfo* info = nullptr);

// 	enum class CircleDirestions
// 	{
// 		None,
// 		Left,
// 		Right,
// 		Back
// 	};

// 	namespace Dodging
// 	{
// 		uint32_t should(RE::Character*);
// 		uint32_t should_danger(RE::Character*);
// 	}

// }
