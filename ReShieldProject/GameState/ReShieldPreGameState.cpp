#include "ReShieldPreGameState.hpp"

#include "GameState/CoreState.hpp"

namespace ReShield
{
	void ReShieldPreGameState::Begin()
	{

	}
	void ReShieldPreGameState::Update()
	{
		_CoreState->GetQuit() = true;
	}
	GameState* ReShieldPreGameState::NextState()
	{
		return nullptr;
	}
	void ReShieldPreGameState::End()
	{

	}
}
