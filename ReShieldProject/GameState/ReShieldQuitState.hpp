#ifndef _RESHIELD_QUIT_STATE_HPP_
#define _RESHIELD_QUIT_STATE_HPP_

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

	class ReShieldQuitState : public GameState
	{
	public:
		ReShieldQuitState(_In_ CoreState* CoreStateObj);

		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

	private:
		CoreState* _CoreState = nullptr;
	};
}

#endif
