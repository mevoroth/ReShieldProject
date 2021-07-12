#ifndef _RESHIELD_INGAME_STATE_HPP_
#define _RESHIELD_INGAME_STATE_HPP_

#include "Core/GameState.hpp"

namespace ReShield
{
	using namespace Eternal::Core;

	class ReShieldInGameStateData;

	class ReShieldInGameState : public GameState
	{
	public:
		ReShieldInGameState(_In_ Game& InGame);
		~ReShieldInGameState();

		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

	private:
		//ReShieldInGameStateData* _ReShieldInGameStateData = nullptr;
	};
}

#endif
