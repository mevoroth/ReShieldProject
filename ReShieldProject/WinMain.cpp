#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/CoreState.hpp"
#include "GameState/ReShieldPreGameState.hpp"

using namespace ReShield;
using namespace Eternal::Core;
using namespace Eternal::GameState;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ReShieldPreGameState* ReShieldPreGameStateObj = new ReShieldPreGameState();
	CoreState CoreStateObj(hInstance, nCmdShow, ReShieldPreGameStateObj);

	ReShieldPreGameStateObj->SetCoreState(&CoreStateObj);

	Game GameObj(&CoreStateObj);
	GameObj.Run();

	delete ReShieldPreGameStateObj;
	ReShieldPreGameStateObj = nullptr;

	return 0;
}
