#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/CoreState.hpp"
#include "GameState/ReShieldPreGameState.hpp"
#include "Data/ReShieldData.hpp"

using namespace ReShield;
using namespace Eternal::Core;
using namespace Eternal::GameState;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	CoreState::CoreStateSettings Settings;
	Settings.ShaderIncludePath = "..\\eternal-engine-shaders\\Shaders\\";
	Settings.FBXPath = "..\\assets\\fbx\\";
	Settings.TexturePath = "..\\assets\\textures\\";
	Settings.SavePath = "..\\save\\";

	ReShieldPreGameState* ReShieldPreGameStateObj = new ReShieldPreGameState();
	CoreState CoreStateObj(Settings, hInstance, nCmdShow, ReShieldPreGameStateObj);
	CoreStateObj.SetSharedData(new ReShieldData());

	Game GameObj(&CoreStateObj);
	GameObj.Run();

	//delete ReShieldPreGameStateObj;
	//ReShieldPreGameStateObj = nullptr;

	return 0;
}
