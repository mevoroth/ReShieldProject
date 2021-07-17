#pragma once

#include "Core/GameState.hpp"

namespace ReShield
{
	using namespace Eternal::Core;

	class ReShieldMainMenuState : public GameState
	{
	public:
		ReShieldMainMenuState(_In_ Game& InGame);

		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;

	private:
		GameState* _NextState = this;

		GameState* _InGameState = nullptr;
	};
}
