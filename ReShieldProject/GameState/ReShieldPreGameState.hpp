#pragma once

#include "Core/GameState.hpp"

namespace ReShield
{
	using namespace Eternal::Core;

	class ReShieldPreGameState : public GameState
	{
	public:
		ReShieldPreGameState(_In_ Game& InGame);

		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

	private:
		GameState* _NextState = this;
		GameState* _MainMenuState = nullptr;
	};
}
