#ifndef _RESHIELD_PRE_GAME_STATE_HPP_
#define _RESHIELD_PRE_GAME_STATE_HPP_

#include "Core/GameState.hpp"

namespace Eternal
{
	namespace GameState
	{
		class CoreState;
	}
}

namespace ReShield
{
	using namespace Eternal::Core;
	using namespace Eternal::GameState;

	class ReShieldPreGameState : public GameState
	{
	public:
		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

		void SetCoreState(_In_ CoreState* CoreStateObj) { _CoreState = CoreStateObj; }

	private:
		CoreState* _CoreState = nullptr;

		GameState* _NextState = this;
		GameState* _MainMenuState = nullptr;
	};
}

#endif
