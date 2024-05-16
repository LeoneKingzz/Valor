#pragma once

namespace Movement
{
	float get_FallbackDistance(RE::Character* me);
	// bool isInDanger(RE::Actor* me, AttackInfo* info = nullptr);
	
	namespace Dodging
	{
		uint32_t should(RE::Character*);
		uint32_t should_danger(RE::Character*);
	}

}

bool has_enoughStamina(RE::Actor* a);
void interruptattack(RE::Actor* me);
