#pragma once

namespace Movement
{
	namespace Fallback
	{
		float get_FallbackDistance(RE::Character* me);
		float get_FallbackSpeed(RE::Character*);
		uint32_t should(RE::Character*);
		float get_FallbackWaitTime(RE::Character*);
	}

	namespace Circle
	{
		float get_CircleAngle(RE::Character* me);
		uint32_t should_danger(RE::Character*);
	}

	uint32_t low_stamina(RE::Character*);
	uint32_t is_indanger(RE::Character*);
	bool is_outofrange(void*);
}
