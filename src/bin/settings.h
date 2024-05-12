#pragma once
#define SQRT2 1.4142135624f
using EventResult = RE::BSEventNotifyControl;

class settings
{
public:
	class update_handler : public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
	public:
		virtual EventResult ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>* a_eventSource);

		static bool Register();
	};
	static void read();
	static void init();

	/*Perilous attack*/
	inline static bool bPerilousAttack_Enable = true;
	static inline bool bPerilousAttack_ChargeTime_Enable = true; /*Beginning of perilous attacks are performed slower to simulate a charging effect and giving the player time to react.*/ 
	static inline float fPerilousAttack_ChargeTime_Mult = 0.1f /*Animation speed during the charge phase.*/;
	static inline float fPerilousAttack_ChargeTime_Duration = 0.5f; /*Duration of the charge phase.*/
	static inline float fPerilousAttack_Chance_Mult = 1.0f;
	/*Perilous bash*/
	static inline bool bPerilousBash_Enable = true;
	static inline float fPerilousBash_ChargeTime_Mult = 0.05f;
	static inline float fPerilousBash_ChargeTime_Duration = 1.0f;

	/*Dodging*/
	inline static bool bDodgeAI_Enable = true; /*Global switch for dodge AI*/
	inline static bool bDodgeAI_Passive_enable = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bDodgeAI_Reactive_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	static inline bool bDodgeAI_AttackCancel_enable = false; /*Cancel attack animations when dodging.*/
	static inline int iDodgeAI_Framework = 1;            /*TKRE = 0; DMCO = 1*/
	
	static inline float fDodgeAI_Chance_Mult = 2.0f; /*Chance multiplier for reactive dodge AI.*/
	static inline float fDodgeAI_Reactive_Dist = 350.0f;               /**/
	static inline float fDodgeAI_DodgeDist = 300.0f;          /*Distance of a single dodge, used for pathing.*/
	static inline float fDodgeAI_DodgeDist2 = fDodgeAI_DodgeDist / SQRT2; /* Cached coordinate distance.*/
	
	static inline float fDodgeAI_DodgeDist_Permissible = 200.0f; /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	static inline bool bDodgeAI_DebugDraw_Enable = false;
	
	/*NPC Commitment*/
	static inline bool bNPCCommitment_Enable = true;
	static inline float fNPCCommitment_AttackStartMult = 1.0f;
	static inline float fNPCCommitment_AttackMidMult = 0.1f;
	static inline float fNPCCommitment_AttackEndMult = 0.f;

	/*Timed block*/
	static inline bool bPerfectBlock_enable = true;
	static inline float fPerfectBlock_chance_mult = 1.0f;

};
