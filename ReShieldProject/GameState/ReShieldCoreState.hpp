#ifndef _RESHIELD_CORE_STATE_HPP_
#define _RESHIELD_CORE_STATE_HPP_

#include "Core/GameState.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

class WindowsProcess;

namespace Eternal
{
	namespace Graphics
	{
		class Device;
		class Renderer;
		class ShaderFactory;
	}
	namespace Input
	{
		class Input;
	}
	namespace Parallel
	{
		class TaskManager;
		class Task;
	}
	namespace Time
	{
		class Time;
	}
}

namespace ReShield
{
	using namespace Eternal::Core;
	using namespace Eternal::Graphics;
	using namespace Eternal::Input;
	using namespace Eternal::Parallel;
	using namespace Eternal::Time;

	class ReShieldCoreState : public GameState
	{
	public:
		ReShieldCoreState(_In_ HINSTANCE hInstance, _In_ int nCmdShow);

		virtual void Begin() override;
		virtual void Update() override;
		virtual GameState* NextState() override;
		virtual void End() override;

	private:
		Time* _Time = nullptr;

		WindowsProcess* _WindowsProcess = nullptr;

		Device* _Device = nullptr;
		HINSTANCE _hInstance = nullptr;
		int _nCmdShow = 0;

		Renderer* _Renderer = nullptr;

		ShaderFactory* _ShaderFactory = nullptr;

		TaskManager* _TaskManager = nullptr;

		Input* _KeyboardInput = nullptr;
		Input* _PadInput = nullptr;

		Task* _ControlsTask = nullptr;
		Task* _ImguiBeginTask = nullptr;
		Task* _ImguiEndTask = nullptr;
		Task* _TimeTask = nullptr;

		void _CreateTasks();
	};
}

#endif
