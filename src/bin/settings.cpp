#include "settings.h"
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
	// bDodgeAI_Reactive_enable = ini.GetBoolValue("General", "bDodgeAI_Reactive_enable", bDodgeAI_Reactive_enable);

	bZUPA_mod_Check = ini.GetBoolValue("General", "bZUPA_mod_Check", bZUPA_mod_Check);
	bUAPNG_mod_Check = ini.GetBoolValue("General", "bUAPNG_mod_Check", bUAPNG_mod_Check);

	auto DS = dodge::GetSingleton();

	DS->Protagnist_Reflexes.Armour_Weighting = static_cast<float>(ini.GetDoubleValue("Protagnist_Reflexes", "Armour_Weighting", DS->Protagnist_Reflexes.Armour_Weighting));
	DS->Protagnist_Reflexes.Defensive_Weighting = static_cast<float>(ini.GetDoubleValue("Protagnist_Reflexes", "Defensive_Weighting", DS->Protagnist_Reflexes.Defensive_Weighting));
	DS->Protagnist_Reflexes.Skirmish_Weighting = static_cast<float>(ini.GetDoubleValue("Protagnist_Reflexes", "Skirmish_Weighting", DS->Protagnist_Reflexes.Skirmish_Weighting));
	DS->Protagnist_Reflexes.Sneak_Weighting = static_cast<float>(ini.GetDoubleValue("Protagnist_Reflexes", "Sneak_Weighting", DS->Protagnist_Reflexes.Sneak_Weighting));

	DS->Armour.Heavyarm_mult = static_cast<float>(ini.GetDoubleValue("Armour", "Heavyarm_mult", DS->Armour.Heavyarm_mult));
	DS->Armour.Lightarm_mult = static_cast<float>(ini.GetDoubleValue("Armour", "Lightarm_mult", DS->Armour.Lightarm_mult));
	DS->Armour.clothing_mult = static_cast<float>(ini.GetDoubleValue("Armour", "clothing_mult", DS->Armour.clothing_mult));

	DS->Armour.Helm_weight = static_cast<float>(ini.GetDoubleValue("Armour", "Helm_weight", DS->Armour.Helm_weight));
	DS->Armour.Chest_weight = static_cast<float>(ini.GetDoubleValue("Armour", "Chest_weight", DS->Armour.Chest_weight));
	DS->Armour.Gauntlet_weight = static_cast<float>(ini.GetDoubleValue("Armour", "Gauntlet_weight", DS->Armour.Gauntlet_weight));
	DS->Armour.Boots_weight = static_cast<float>(ini.GetDoubleValue("Armour", "Boots_weight", DS->Armour.Boots_weight));
	DS->Armour.Shield_weight = static_cast<float>(ini.GetDoubleValue("Armour", "Shield_weight", DS->Armour.Shield_weight));

	DS->CStyle.Skirmish_AvoidThreat_Weighting = static_cast<float>(ini.GetDoubleValue("CombatStyle", "Skirmish_AvoidThreat_Weighting", DS->CStyle.Skirmish_AvoidThreat_Weighting));
	DS->CStyle.Skirmish_Circle_Weighting = static_cast<float>(ini.GetDoubleValue("CombatStyle", "Skirmish_Circle_Weighting", DS->CStyle.Skirmish_Circle_Weighting));
	DS->CStyle.Skirmish_Fallback_Weighting = static_cast<float>(ini.GetDoubleValue("CombatStyle", "Skirmish_Fallback_Weighting", DS->CStyle.Skirmish_Fallback_Weighting));
	DS->CStyle.Skirmish_Strafe_Weighting = static_cast<float>(ini.GetDoubleValue("CombatStyle", "Skirmish_Strafe_Weighting", DS->CStyle.Skirmish_Strafe_Weighting));

	(void)ini.SaveFile(a_path);
	
}

void settings::setglobals(){

	auto HdSingle = RE::TESDataHandler::GetSingleton();
	auto UND_fSideStep_staminacost = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80B, "Ultimate NPC Dodging.esp"));
	auto UND_bReactiveDodgeAI_enable = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80C, "Ultimate NPC Dodging.esp"));
	auto UND_fDodgeRoll_staminacost = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80D, "Ultimate NPC Dodging.esp"));
	auto UND_bHasSilentRollperk_enable = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80E, "Ultimate NPC Dodging.esp"));
	auto UND_iDodgeRoll_ActorScaled_Chance = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x80F, "Ultimate NPC Dodging.esp"));

	UND_fSideStep_staminacost->value = fSideStep_staminacost;
	UND_fDodgeRoll_staminacost->value = fDodgeRoll_staminacost;
	UND_bReactiveDodgeAI_enable->value = static_cast<float>(iReactiveDodgeAI_enable);
	UND_bHasSilentRollperk_enable->value = static_cast<float>(bHasSilentRollperk_enable);
	UND_iDodgeRoll_ActorScaled_Chance->value = static_cast<float>(iDodgeRoll_ActorScaled_Chance);
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
		settings::read(L"Data/SKSE/Plugins/UltimateNPCDodging.ini");
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
