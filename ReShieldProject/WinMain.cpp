#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/CoreState.hpp"

using namespace Eternal::Core;
using namespace Eternal::GameState;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	CoreState CoreStateObj(hInstance, nCmdShow);

	Game GameObj(&CoreStateObj);
	GameObj.Run();

	return 0;
}
