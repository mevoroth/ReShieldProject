#include "ReShieldMainMenuState.hpp"

#include "ReShield.hpp"

#include "Input/Input.hpp"

#include "GameState/CoreState.hpp"
#include "GameState/ReShieldInGameState.hpp"
#include "GameState/ReShieldQuitState.hpp"
#include "Core/System.hpp"
#include "Log/Log.hpp"

namespace ReShield
{
	ReShieldMainMenuState::ReShieldMainMenuState(_In_ Game& InGame)
		: GameState(InGame)
	{
	}

	void ReShieldMainMenuState::Begin()
	{
		_InGameState = new ReShieldInGameState(GetGame());
	}
	void ReShieldMainMenuState::Update()
	{
		using namespace Eternal;
		using namespace Eternal::InputSystem;

		LogWrite(LogInfo, ReShield::ReShieldGame, "[ReShieldMainMenuState::Update]");
		
		if (GetSystem().GetInput().IsDown(Input::ESC))
		{
			_NextState = nullptr;
		}
		else if (GetSystem().GetInput().IsDown(Input::A))
		{
			_NextState = _InGameState;
		}
	}
	GameState* ReShieldMainMenuState::NextState()
	{
		return _NextState;
	}
	void ReShieldMainMenuState::End()
	{

	}
}
