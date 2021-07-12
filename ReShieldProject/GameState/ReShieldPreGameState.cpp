#include "ReShieldPreGameState.hpp"

#include "ReShield.hpp"

#include "Input/Input.hpp"
#include "Core/System.hpp"
#include "GameState/CoreState.hpp"
#include "GameState/ReShieldMainMenuState.hpp"

namespace ReShield
{
	ReShieldPreGameState::ReShieldPreGameState(_In_ Game& InGame)
		: GameState(InGame)
	{
	}

	void ReShieldPreGameState::Begin()
	{
		_MainMenuState = new ReShieldMainMenuState(GetGame());
	}
	void ReShieldPreGameState::Update()
	{
		using namespace Eternal;
		using namespace Eternal::InputSystem;

		LogWrite(LogInfo, ReShield::ReShieldGame, "[ReShieldPreGameState::Update]");
		if (GetSystem().GetInput().IsDown(Input::ESC))
		{
			_NextState = _MainMenuState;
		}
	}
	GameState* ReShieldPreGameState::NextState()
	{
		return _NextState;
	}
	void ReShieldPreGameState::End()
	{
		delete this;
	}
}
