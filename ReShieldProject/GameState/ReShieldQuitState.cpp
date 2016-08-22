#include "ReShieldQuitState.hpp"

namespace ReShield
{
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
		delete this;
	}
}
