#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\UltimateNPCDodging.ini"

#define FETCH(setting) load(setting, #setting)
void settings::read()
{
	DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	reader.setActiveSection("General");
	reader.FETCH(bRecoilStunBreak_enable);
	reader.FETCH(biFrames_enable);
	reader.FETCH(bHasSilentRollperk_enable);
	reader.FETCH(fSideStep_staminacost);
	reader.FETCH(fDodgeRoll_staminacost);
	reader.FETCH(bStaminaCost_enable);
	reader.FETCH(iDodgeRoll_ActorScaled_Chance);
	reader.FETCH(bTacticalDodgeAI_enable);
	reader.FETCH(bZUPA_mod_Check);
	reader.FETCH(bUAPNG_mod_Check);

	// reader.FETCH(bDodgeAI_DebugDraw_Enable);

	reader.log();
	
}

void settings::init()
{
	update_handler::Register();
}

inline EventResult settings::update_handler::ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
{
	if (!a_event) {
		return EventResult::kContinue;
	}
	if (a_event->eventName == "dmenu_updateSettings" && a_event->strArg == "UND") {
		settings::read();
	}

	return EventResult::kContinue;
}

inline bool settings::update_handler::Register()
{
	static update_handler singleton;

	auto eventSource = SKSE::GetModCallbackEventSource();

	if (!eventSource) {
		/*ERROR("EventSource not found!");*/
		return false;
	}
	eventSource->AddEventSink(&singleton);
	logger::info("Register {}", typeid(singleton).name());
	return true;
}
