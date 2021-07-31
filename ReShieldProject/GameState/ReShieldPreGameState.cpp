#include "ReShieldPreGameState.hpp"

#include "ReShield.hpp"

#include "Input/Input.hpp"
#include "Core/System.hpp"
#include "GameState/ReShieldMainMenuState.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/LevelPayload.hpp"

namespace ReShield
{
	using namespace Eternal::Import;
	using namespace Eternal::Resources;

	ReShieldPreGameState::ReShieldPreGameState(_In_ Game& InGame)
		: GameState(InGame)
	{
	}

	void ReShieldPreGameState::Begin()
	{
		_MainMenuState = new ReShieldMainMenuState(GetGame());

		LevelRequest* Request = new LevelRequest("Sponza.json");
		GetSystem().GetStreaming().EnqueueRequest(Request);
	}

	void ReShieldPreGameState::Update()
	{
		using namespace Eternal::InputSystem;

		ImGui::Begin("Splashscreen");
		bool IsPressed = ImGui::Button("To menu (ESC)");
		ImGui::End();

		if (IsPressed || GetSystem().GetInput().IsDown(Input::ESC))
		{
			_NextState = _MainMenuState;
		}
	}

	void ReShieldPreGameState::UpdateDebug()
	{
		using namespace Eternal;
		//LogWrite(LogInfo, ReShield::ReShieldGame, "[ReShieldPreGameState::Update]");

		bool IsWindowOpen = true;
		ImGui::ShowMetricsWindow(&IsWindowOpen);

		static uint32_t FrameIndex = 0;

		ImGui::Begin("Debug");
		ImGui::Text("Frame Index [%d]", FrameIndex++);
		ImGui::End();
	}

	GameState* ReShieldPreGameState::NextState()
	{
		return _NextState;
	}
}
