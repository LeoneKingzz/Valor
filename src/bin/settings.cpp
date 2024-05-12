#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\Valor\\Settings.ini"

#define FETCH(setting) load(setting, #setting)
void settings::read()
{
	DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	reader.setActiveSection("Dodge");
	reader.FETCH(bDodgeAI_Enable);
	reader.FETCH(bDodgeAI_Passive_enable);
	reader.FETCH(bDodgeAI_Reactive_enable);
	reader.FETCH(bDodgeAI_AttackCancel_enable);
	
	reader.FETCH(iDodgeAI_Framework);
	reader.FETCH(fDodgeAI_DodgeDist);
	fDodgeAI_DodgeDist2 = fDodgeAI_DodgeDist / SQRT2;
	reader.FETCH(fDodgeAI_DodgeDist_Permissible);
	reader.FETCH(fDodgeAI_Reactive_Dist);
	reader.FETCH(fDodgeAI_Chance_Mult);
	
	reader.FETCH(bDodgeAI_DebugDraw_Enable);

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
	if (a_event->eventName == "dmenu_updateSettings" && a_event->strArg == "Valor") {
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
