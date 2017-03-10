#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/CoreState.hpp"
#include "GameState/ReShieldPreGameState.hpp"
#include "Data/ReShieldData.hpp"

#include "Macros/Macros.hpp"
#include "Window/Window.hpp"
//#include "d3d12/D3D12Device.hpp"
//#include "d3d12/D3D12State.hpp"
//#include "d3d12/D3D12InputLayout.hpp"
//#include "d3d12/D3D12ShaderFactory.hpp"
//#include "d3d12/D3D12Shader.hpp"
//#include "d3d12/D3D12CommandQueue.hpp"
//#include "d3d12/D3D12CommandList.hpp"
//#include "d3d12/D3D12Resource.hpp"
//#include "d3d12/D3D12RenderTarget.hpp"
//#include "d3d12/D3D12Fence.hpp"
#include "Graphics/DepthTest.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/Comparison.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/ShaderType.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
#include "Vulkan/VulkanCommandList.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanState.hpp"
#include "Vulkan/VulkanShader.hpp"
#include "Vulkan/VulkanCommandQueue.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanFence.hpp"

#include "File/FilePath.hpp"

#include <dxgi1_4.h>
#include <vector>

using namespace ReShield;
using namespace Eternal::Core;
using namespace Eternal::GameState;

using namespace Eternal::Graphics;
using namespace Eternal::File;
using namespace std;

LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT PaintStruct;
		HDC Hdc = BeginPaint(hWnd, &PaintStruct);
		EndPaint(hWnd, &PaintStruct);
	} break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Window WindowObj(hInstance, nCmdShow, "Vulkan", "Vulkan", 1280, 720);
	WindowObj.Create(WindowProc);
	VulkanDevice DeviceObj(WindowObj);
	VulkanSwapChain SwapChainObj(DeviceObj, WindowObj);

	VulkanCommandQueue DirectCommandQueue(DeviceObj, SwapChainObj);

	FilePath::Register("..\\eternal-engine-shaders\\Shaders\\", FilePath::SHADERS);

	Viewport ViewportObj(0, 0, 1280, 720);

	VulkanShader VS(DeviceObj, "PostProcessVS", "postprocess.vs.spirv", VS);
	VulkanShader PS(DeviceObj, "DefaultPostProcessPS", "postprocess.ps.spirv", PS);

	//VulkanCommandList CommandList(DeviceObj, DirectCommandQueue.GetCommandAllocator());
	VulkanCommandList CommandLists[] = {
		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(0)),
		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(1))
	};

	VulkanFence FenceObj(DeviceObj, 2);
	VulkanPipeline Pipeline(DeviceObj);

	VulkanState StateObj(DeviceObj, Pipeline, VS, PS, ViewportObj);

	static int i = 0;
	for (;;)
	{
		FenceObj.Wait(DeviceObj);
		FenceObj.Reset(DeviceObj);
		uint32_t CurrentFrame = SwapChainObj.AcquireFrame(DeviceObj, FenceObj);
		DirectCommandQueue.Reset(CurrentFrame);
		CommandLists[CurrentFrame].Begin(SwapChainObj.GetBackBuffer(CurrentFrame), StateObj, Pipeline, *SwapChainObj.GetMainRenderPass());
		CommandLists[CurrentFrame].DrawPrimitive(6);
		CommandLists[CurrentFrame].End();
		//FenceObj.Reset(DeviceObj);
		//DirectCommandQueue.Flush(FenceObj, SwapChainObj, &CommandList, 1);
		CommandList* VulkanCommandLists[] = {
			&CommandLists[CurrentFrame]
		};
		FenceObj.Signal(SwapChainObj, DirectCommandQueue, VulkanCommandLists, 1);
		SwapChainObj.Present(DeviceObj, DirectCommandQueue, CurrentFrame);

		MSG Message = { 0 };
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		i++;
	}

	//WindowObj.Create(WindowProc);

	//D3D12Device::Initialize();
	//D3D12Device DeviceObj(0);

	//DeviceObj.CreateSwapChain(WindowObj);

	//InputLayout::VertexDataType Vertices[] = {
	//	InputLayout::POSITION_T
	//};

	//D3D12ShaderFactory* ShaderFactoryObj = new D3D12ShaderFactory();

	//Viewport ViewportObj(0, 0, 1280, 720);

	//D3D12InputLayout InputLayoutObj(Vertices, ETERNAL_ARRAYSIZE(Vertices));
	//D3D12ShaderFactory::Get()->RegisterShaderPath("..\\eternal-engine-shaders\\Shaders\\");
	//Shader* VS = D3D12ShaderFactory::Get()->CreateVertexShader("PostProcess", "postprocess.vs.hlsl");
	//Shader* PS = D3D12ShaderFactory::Get()->CreatePixelShader("DefaultPostProcess", "defaultpostprocess.ps.hlsl");
	//DepthTest DepthTestObj;
	//StencilTest StencilTestObj;
	//BlendState BlendStates[] = {
	//	BlendState(BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD, BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD),
	//	BlendState(),
	//	BlendState(),
	//	BlendState(),
	//	BlendState(),
	//	BlendState(),
	//	BlendState(),
	//	BlendState()
	//};

	//D3D12State State(DeviceObj, InputLayoutObj, VS, PS, DepthTestObj, StencilTestObj, BlendStates, DeviceObj.GetBackBuffer(0), 1, nullptr, 0);
	//D3D12CommandList CommandList(DeviceObj, *DeviceObj.GetCommandQueue(), State);
	//D3D12Fence Fence(DeviceObj, 2);

	//const float ClearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//int i = 0;
	//int CurrentFrame = 0;
	//for (;;)
	//{
	//	CurrentFrame = DeviceObj.GetSwapChain3()->GetCurrentBackBufferIndex();
	//	Fence.Wait();

	//	DeviceObj.GetCommandQueue()->Reset(CurrentFrame);

	//	CommandList.Begin(*DeviceObj.GetCommandQueue()->GetCommandAllocator(CurrentFrame), State);
	//	CommandList.SetViewport(ViewportObj);
	//	CommandList.SetScissorRectangle(ViewportObj);
	//	DeviceObj.GetBackBuffer(CurrentFrame)->Transition(CommandList, RENDERTARGET);
	//	CommandList.BindRenderTarget(0, *DeviceObj.GetBackBuffer(CurrentFrame));
	//	CommandList.GetD3D12GraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	//CommandList.GetD3D12GraphicsCommandList()->ClearRenderTargetView(DeviceObj.GetBackBuffer(CurrentFrame)->GetCpuDescriptor(), ClearColor, 0, nullptr);
	//	CommandList.DrawPrimitive(6);
	//	DeviceObj.GetBackBuffer(CurrentFrame)->Transition(CommandList, PRESENT);
	//	CommandList.End();

	//	DeviceObj.GetCommandQueue()->GetD3D12CommandQueue()->ExecuteCommandLists(1, (ID3D12CommandList* const*)&CommandList.GetD3D12GraphicsCommandList());
	//	DeviceObj.GetSwapChain()->Present(0, 0);
	//	Fence.Signal(*DeviceObj.GetCommandQueue());

	//	MSG Message = { 0 };
	//	if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	//	{
	//		TranslateMessage(&Message);
	//		DispatchMessage(&Message);
	//	}
	//	i = i ^ 1;
	//}

	//CoreState::CoreStateSettings Settings;
	//Settings.ShaderIncludePath = "..\\eternal-engine-shaders\\Shaders\\";
	//Settings.FBXPath = "..\\assets\\fbx\\";
	//Settings.TexturePath = "..\\assets\\textures\\";
	//Settings.SavePath = "..\\save\\";

	//ReShieldPreGameState* ReShieldPreGameStateObj = new ReShieldPreGameState();
	//CoreState CoreStateObj(Settings, hInstance, nCmdShow, ReShieldPreGameStateObj);
	//CoreStateObj.SetSharedData(new ReShieldData());

	//Game GameObj(&CoreStateObj);
	//GameObj.Run();

	////delete ReShieldPreGameStateObj;
	////ReShieldPreGameStateObj = nullptr;

	return 0;
}
