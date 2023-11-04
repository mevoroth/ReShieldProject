#include "DebugTools/Debug.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Platform/WindowsProcess.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "GameState/ReShieldPreGameState.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	using namespace ReShield;
	using namespace Eternal::Graphics;
	using namespace Eternal::Platform;
	using namespace Eternal::Core;

	Eternal::DebugTools::WaitForDebugger();
	
	const DeviceType ProgramDeviceType = DeviceType::VULKAN;

	const char* AppName = ProgramDeviceType == DeviceType::D3D12 ? "D3D12" : "Vulkan";

	//OPTICK_APP(AppName);

	RenderSettings Settings(
		ProgramDeviceType,
		1600, 900,
		/*InIsVSync =*/ true
	);
	WindowsArguments WinArguments(
		hInstance,
		hPrevInstance,
		lpCmdLine,
		nCmdShow,
		AppName,
		AppName,
		WindowsProcess::WindowProc
	);
	GraphicsContextCreateInformation ContextCreateInformation(Settings, WinArguments);
	
	SystemCreateInformation SystemInformation(ContextCreateInformation);
	SystemInformation.FBXPath			= "..\\assets\\fbx\\";
	SystemInformation.FBXCachePath		= "..\\assets\\fbx\\cache\\";
	SystemInformation.ShaderIncludePath	= { "..\\eternal-engine-shaders\\Shaders\\",
											"..\\eternal-engine-shaders\\" };
	SystemInformation.ShaderPDBPath		= "..\\assets\\shaders_pdb\\";
	SystemInformation.TexturePath		= "..\\assets\\textures\\";
	SystemInformation.LevelPath			= "..\\assets\\scenes\\";

	GameCreateInformation GameInformation(SystemInformation);

	StartGame<ReShieldPreGameState> EternalGame(GameInformation);
	EternalGame.Run();

	return 0;
}
