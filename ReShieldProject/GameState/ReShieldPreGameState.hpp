#ifndef _RESHIELD_PRE_GAME_STATE_HPP_
#define _RESHIELD_PRE_GAME_STATE_HPP_

#include "Core/GameState.hpp"

namespace ReShield
{
	using namespace Eternal::Core;

	class ReShieldPreGameState : public GameState
	{
	public:
		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

	private:
		GameState* _NextState = this;
		GameState* _MainMenuState = nullptr;
	};
}

#endif
