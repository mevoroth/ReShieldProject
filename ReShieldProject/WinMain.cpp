#include "DebugTools/Debug.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Platform/WindowsProcess.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "GameState/ReShieldPreGameState.hpp"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	using namespace ReShield;
	using namespace Eternal::Graphics;
	using namespace Eternal::Platform;
	using namespace Eternal::Core;

	Eternal::DebugTools::WaitForDebugger();
	
	const DeviceType ProgramDeviceType = DeviceType::VULKAN;

	const char* AppName = ProgramDeviceType == DeviceType::D3D12 ? "D3D12" : "Vulkan";

	OPTICK_APP(AppName);

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
	SystemInformation.ShaderIncludePath	= "..\\eternal-engine-shaders\\Shaders\\";
	SystemInformation.TexturePath		= "..\\assets\\textures\\";

	GameCreateInformation GameInformation(SystemInformation);

	StartGame<ReShieldPreGameState> EternalGame(GameInformation);
	EternalGame.Run();

	return 0;
}
