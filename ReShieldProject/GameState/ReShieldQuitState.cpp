#include "ReShieldQuitState.hpp"

namespace ReShield
{
	ReShieldQuitState::ReShieldQuitState(_In_ CoreState* CoreStateObj)
		: _CoreState(CoreStateObj)
	{
	}

	void ReShieldQuitState::Begin()
	{

	}
	void ReShieldQuitState::Update()
	{

	}
	GameState* ReShieldQuitState::NextState()
	{
		return nullptr;
	}
	void ReShieldQuitState::End()
	{

	}
}
