#include "ReShieldMainMenuState.hpp"

#include "ReShield.hpp"

#include "Input/Input.hpp"

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

		//LogWrite(LogInfo, ReShield::ReShieldGame, "[ReShieldMainMenuState::Update]");

		ImGui::Begin("Main menu");
		bool IsPlayPressed = ImGui::Button("Play (ENTER)");
		bool IsQuitPressed = ImGui::Button("Quit (ESC)");
		ImGui::End();

		if (IsQuitPressed || GetSystem().GetInput().IsDown(Input::ESC))
		{
			_NextState = nullptr;
		}
		else if (IsPlayPressed || GetSystem().GetInput().IsDown(Input::RETURN))
		{
			_NextState = _InGameState;
		}
	}
	GameState* ReShieldMainMenuState::NextState()
	{
		return _NextState;
	}
}
