#include "AttackState.h"
#include "settings.h"
#include "include/Utils.h"
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;

void AttackState::clean_atk_state(RE::ActorHandle a_handle)
{
	if (!settings::bNPCCommitment_Enable) {
		return;
	}
	writeLock l(_atk_state_map_lock);
	_atk_state_map.erase(a_handle);
}

void AttackState::set_atk_state(RE::ActorHandle a_handle, atk_state a_state)
{
	if (!settings::bNPCCommitment_Enable) {
		return;
	}
	writeLock l(_atk_state_map_lock);
	_atk_state_map[a_handle] = a_state;
}

AttackState::atk_state AttackState::get_atk_state(RE::ActorHandle a_handle)
{
	if (!settings::bNPCCommitment_Enable) {
		return kNone;
	}
	readLock l(_atk_state_map_lock);
	return _atk_state_map.contains(a_handle) ? _atk_state_map[a_handle] : kNone;
}
