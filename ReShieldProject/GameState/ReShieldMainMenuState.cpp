#include "ReShieldMainMenuState.hpp"

#include "ReShield.hpp"

#include "Input/Input.hpp"

#include "GameState/CoreState.hpp"
#include "GameState/ReShieldInGameState.hpp"
#include "GameState/ReShieldQuitState.hpp"

namespace ReShield
{
	void ReShieldMainMenuState::Begin()
	{
		_InGameState = new ReShieldInGameState();
	}
	void ReShieldMainMenuState::Update()
	{
		using namespace Eternal;

		LogWrite(LogInfo, ReShield::ReShieldGame, "[ReShieldMainMenuState::Update]");
		if (Eternal::Input::Input::Get()->IsDown(Eternal::Input::Input::ESC))
		{
			_NextState = nullptr;
		}
		else if (Eternal::Input::Input::Get()->IsDown(Eternal::Input::Input::A))
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
