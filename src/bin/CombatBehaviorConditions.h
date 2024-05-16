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






	
	float get_FallbackWaitTime(RE::Character*);
	float get_CircleAngle(RE::Character* me);
	uint32_t low_stamina(RE::Character*);
	bool is_outofrange(void*);
}
