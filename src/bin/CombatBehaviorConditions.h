#pragma once

namespace Movement
{
	float get_FallbackDistance(RE::Character* me);
	
	namespace Dodging
	{
		uint32_t should(RE::Character*);
		uint32_t should_danger(RE::Character*);
		uint32_t is_indanger(RE::Character*);
	}

}

bool has_enoughStamina(RE::Actor* a);
void interruptattack(RE::Actor* me);