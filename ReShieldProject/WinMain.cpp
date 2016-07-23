#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/ReShieldCoreState.hpp"

using namespace ReShield;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ReShieldCoreState ReShieldInitStateObj(hInstance, nCmdShow);

	Game GameObj(&ReShieldInitStateObj);
	GameObj.Run();

	return 0;
}
