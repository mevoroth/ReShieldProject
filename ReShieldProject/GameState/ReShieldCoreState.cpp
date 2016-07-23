#include "ReShieldCoreState.hpp"

#include "Windows/WindowsProcess.hpp"
#include "Graphics/DeviceFactory.hpp"
#include "Graphics/RendererFactory.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderFactoryFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Time/TimeFactory.hpp"
#include "Parallel/TaskManager.hpp"
#include "Task/ControlsTask.hpp"
#include "Task/ImguiBeginTask.hpp"
#include "Task/ImguiEndTask.hpp"
#include "Task/TimeTask.hpp"

namespace ReShield
{
	using namespace Eternal::Task;

	ReShieldCoreState::ReShieldCoreState(_In_ HINSTANCE hInstance, _In_ int nCmdShow)
		: _hInstance(hInstance)
		, _nCmdShow(nCmdShow)
	{
	}

	void ReShieldCoreState::Begin()
	{
		_Time = CreateTime(TimeType::WIN);

		_KeyboardInput = CreateInput(InputType::WIN);
		_PadInput = CreateInput(XINPUT);

		WindowsProcess::SetInputHandler(_KeyboardInput);
		_WindowsProcess = new WindowsProcess();

		_Device = CreateDevice(WINDOWS, WindowsProcess::WindowProc, _hInstance, _nCmdShow, "ReShield", "EternalClass");
		_Renderer = CreateRenderer(RENDERER_D3D11);

		_ShaderFactory = CreateShaderFactory(SHADER_FACTORY_D3D11);

		_TaskManager = new TaskManager();
		
		_CreateTasks();

		_TaskManager->GetTaskScheduler().PushTask(_ControlsTask);
		_TaskManager->GetTaskScheduler().PushTask(_TimeTask);
		{
			_TaskManager->GetTaskScheduler().PushTask(_ImguiBeginTask, _ControlsTask);
			_TaskManager->GetTaskScheduler().PushTask(_ImguiBeginTask, _TimeTask);
		}
		_TaskManager->GetTaskScheduler().PushTask(_ImguiEndTask, _ImguiBeginTask);
	}
	void ReShieldCoreState::Update()
	{
		for (;;)
		{
			WindowsProcess::ExecuteMessageLoop();

			_TaskManager->Schedule();
			_TaskManager->Barrier();
			_TaskManager->GetTaskScheduler().Reset();
		}
	}
	GameState* ReShieldCoreState::NextState()
	{
		return nullptr;
	}
	void ReShieldCoreState::End()
	{

	}

	void ReShieldCoreState::_CreateTasks()
	{
		ControlsTask* ControlsTaskObj = new ControlsTask();
		ControlsTaskObj->SetTaskName("Controls Task");
		ControlsTaskObj->RegisterInput(_PadInput);
		ControlsTaskObj->RegisterInput(_KeyboardInput);
		_ControlsTask = ControlsTaskObj;

		ImguiBeginTask* ImguiBeginTaskObj = new ImguiBeginTask(_Device->GetWindow(), Device::WIDTH, Device::HEIGHT);
		ImguiBeginTaskObj->SetTaskName("Imgui Begin Task");
		ImguiBeginTaskObj->SetupKeyboard(_KeyboardInput);
		_ImguiBeginTask = ImguiBeginTaskObj;

		ImguiEndTask* ImguiEndTaskObj = new ImguiEndTask(*_Renderer->GetMainContext());
		ImguiEndTaskObj->SetTaskName("Imgui End Task");
		_ImguiEndTask = ImguiEndTaskObj;

		TimeTask* TimeTaskObj = new TimeTask(_Time);
		TimeTaskObj->SetTaskName("Time Task");
		_TimeTask = TimeTaskObj;
	}
}
