#ifndef _RESHIELD_MAIN_MENU_STATE_HPP_
#define _RESHIELD_MAIN_MENU_STATE_HPP_

#include "Core/GameState.hpp"

namespace ReShield
{
	using namespace Eternal::Core;

	class ReShieldMainMenuState : public GameState
	{
	public:
		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;
	};
}

#endif
