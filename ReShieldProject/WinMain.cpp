#include "DebugTools/Debug.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Platform/Windows/WindowsProcess.hpp"
#include "Windows/WindowsArguments.hpp"
#include "Windows/WindowsGraphicsContext.hpp"
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
	
	const DeviceType ProgramDeviceType = DeviceType::DEVICE_TYPE_DEFAULT;

	const char* AppName = ProgramDeviceType == DeviceType::DEVICE_TYPE_D3D12 ? "D3D12" : "Vulkan";

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
	WindowsGraphicsContextCreateInformation ContextCreateInformation(Settings, WinArguments);
	
	SystemCreateInformation SystemInformation(ContextCreateInformation);
	SystemInformation.FBXPath			= "..\\assets\\fbx\\";
	SystemInformation.FBXCachePath		= "..\\assets\\fbx\\cache\\";
	SystemInformation.ShaderIncludePath	= { "..\\eternal-engine-shaders\\Shaders\\",
											"..\\eternal-engine-shaders\\" };
	SystemInformation.ShaderPDBPath		= "..\\assets\\shaders_pdb\\";
	SystemInformation.TexturePath		= "..\\assets\\textures\\";
	SystemInformation.LevelPath			= "..\\assets\\scenes\\";
	SystemInformation.PipelineCachePath	= "..\\assets\\pipelines\\";
	SystemInformation.MaterialPath		= "..\\assets\\materials\\";

	GameCreateInformation GameInformation(SystemInformation);

	StartGame<ReShieldPreGameState> EternalGame(GameInformation);
	EternalGame.Run();

	return 0;
}
