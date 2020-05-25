#include "ReShieldPreGameState.hpp"

#include "ReShield.hpp"

#include "Macros/Macros.hpp"
#include "Input/Input.hpp"

#include "GameState/CoreState.hpp"
#include "GameState/ReShieldMainMenuState.hpp"

namespace ReShield
{
	using namespace Eternal::Log;

	void ReShieldPreGameState::Begin()
	{
		_MainMenuState = new ReShieldMainMenuState();
	}
	void ReShieldPreGameState::Update()
	{
		//Log::Get()->Write(Log::Info, ReShield::ReShieldGame, "[ReShieldPreGameState::Update]");
		if (Eternal::Input::Input::Get()->IsDown(Eternal::Input::Input::ESC))
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
