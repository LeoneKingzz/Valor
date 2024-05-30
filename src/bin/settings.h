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


	/*Dodging*/
	inline static bool bDodgeAI_Enable = true; /*Global switch for dodge AI*/
	inline static bool bDodgeAI_Passive_enable = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bDodgeAI_Reactive_enable = false; /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool bRecoilStunBreak_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	static inline bool bDodgeAI_AttackCancel_enable = false; /*Cancel attack animations when dodging.*/
	static inline int iDodgeAI_Framework = 0;            /*TKRE = 0; DMCO = 1*/
	
	static inline float fDodgeAI_Chance_Mult = 1.0f; /*Chance multiplier for reactive dodge AI.*/
	static inline float fDodgeAI_Reactive_Dist = 350.0f;               /**/
	static inline float fDodgeAI_DodgeDist = 300.0f;          /*Distance of a single dodge, used for pathing.*/
	static inline float fDodgeAI_DodgeDist2 = fDodgeAI_DodgeDist / SQRT2; /* Cached coordinate distance.*/
	
	static inline float fDodgeAI_DodgeDist_Permissible = 200.0f; /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	static inline bool bDodgeAI_DebugDraw_Enable = false;


};
