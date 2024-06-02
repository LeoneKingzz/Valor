#include "settings.h"
#include "include/Utils.h"
// #define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\UltimateNPCDodging.ini"

// #define FETCH(setting) load(setting, #setting)
void settings::read(const wchar_t* a_path)
{
	// DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	// reader.setActiveSection("General");
	
	// reader.FETCH(biFrames_enable);
	// reader.FETCH(bHasSilentRollperk_enable);
	// reader.FETCH(bStaminaCost_enable);
	// reader.FETCH(bTacticalDodgeAI_enable);
	// reader.FETCH(bZUPA_mod_Check);
	// reader.FETCH(bUAPNG_mod_Check);

	// reader.FETCH(fSideStep_staminacost);
	// reader.FETCH(fDodgeRoll_staminacost);
	// reader.FETCH(iDodgeRoll_ActorScaled_Chance);

	// reader.log();

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(a_path);
	const auto get_value = [&]<class T>(T& a_value, const char* a_section, const char* a_key, const char* a_comment) {
		if constexpr (std::is_same_v<bool, T>) {
			a_value = ini.GetBoolValue(a_section, a_key, a_value);
			ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		} else {
			a_value = static_cast<float>(ini.GetDoubleValue(a_section, a_key, a_value));
			ini.SetDoubleValue(a_section, a_key, a_value, a_comment);
		}
	};

	fSideStep_staminacost = static_cast<float>(ini.GetDoubleValue("General", "fSideStep_staminacost", fSideStep_staminacost));
	fDodgeRoll_staminacost = static_cast<float>(ini.GetDoubleValue("General", "fDodgeRoll_staminacost", fDodgeRoll_staminacost));
	iDodgeRoll_ActorScaled_Chance = (ini.GetLongValue("General", "iDodgeRoll_ActorScaled_Chance", iDodgeRoll_ActorScaled_Chance));
	iReactiveDodgeAI_enable = (ini.GetLongValue("General", "iReactiveDodgeAI_enable", iReactiveDodgeAI_enable));

	biFrames_enable = ini.GetBoolValue("General", "biFrames_enable", biFrames_enable);
	bHasSilentRollperk_enable = ini.GetLongValue("General", "bHasSilentRollperk_enable", bHasSilentRollperk_enable);
	bStaminaCost_enable = ini.GetBoolValue("General", "bStaminaCost_enable", bStaminaCost_enable);
	bTacticalDodgeAI_enable = ini.GetBoolValue("General", "bTacticalDodgeAI_enable", bTacticalDodgeAI_enable);
	bZUPA_mod_Check = ini.GetBoolValue("General", "bZUPA_mod_Check", bZUPA_mod_Check);
	bUAPNG_mod_Check = ini.GetBoolValue("General", "bUAPNG_mod_Check", bUAPNG_mod_Check);

	auto HdSingle = RE::TESDataHandler::GetSingleton();

	auto UND_fSideStep_staminacost = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80B, "Ultimate NPC Dodging.esp"));
	auto UND_bReactiveDodgeAI_enable = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80C, "Ultimate NPC Dodging.esp"));
	auto UND_fDodgeRoll_staminacost = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80D, "Ultimate NPC Dodging.esp"));
	auto UND_bHasSilentRollperk_enable = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80E, "Ultimate NPC Dodging.esp"));
	auto UND_iDodgeRoll_ActorScaled_Chance = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80F, "Ultimate NPC Dodging.esp"));

	UND_fSideStep_staminacost->value = fSideStep_staminacost;
	UND_bReactiveDodgeAI_enable->value = iReactiveDodgeAI_enable;
	UND_bHasSilentRollperk_enable->value = bHasSilentRollperk_enable;
	UND_iDodgeRoll_ActorScaled_Chance->value = iDodgeRoll_ActorScaled_Chance;
	UND_fDodgeRoll_staminacost->value = fDodgeRoll_staminacost;
	
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
