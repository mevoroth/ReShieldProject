#include <Windows.h>
#include "Core/Main.hpp"
#include "GameState/ReShieldPreGameState.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	using namespace Eternal::Core;

	//OPTICK_APP(AppName);

	MainInput Input;
	Input.ApplicationName	= "ReShield";
	Input.hInstance			= hInstance;
	Input.hPrevInstance		= hPrevInstance;
	Input.lpCmdLine			= lpCmdLine;
	Input.nCmdShow			= nCmdShow;

	return Main<ReShield::ReShieldPreGameState>(Input);
}
