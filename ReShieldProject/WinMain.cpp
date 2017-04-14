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

#include "Graphics/DepthTest.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/Comparison.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/ShaderType.hpp"

/*
#include "d3d12/D3D12Device.hpp"
#include "d3d12/D3D12Pipeline.hpp"
#include "d3d12/D3D12InputLayout.hpp"
#include "d3d12/D3D12ShaderFactory.hpp"
#include "d3d12/D3D12Shader.hpp"
#include "d3d12/D3D12CommandList.hpp"
#include "d3d12/D3D12Resource.hpp"
#include "d3d12/D3D12RenderTarget.hpp"
/*/
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanCommandList.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanShader.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanRootSignature.hpp"
#include "Vulkan/VulkanResource.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
//*/
#include "Graphics/CommandQueueFactory.hpp"
#include "Graphics/SwapChainFactory.hpp"
#include "Graphics/FenceFactory.hpp"
#include "Graphics/Fence.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/SwapChain.hpp"
#include "Graphics/RootSignature.hpp"
#include "Graphics/RootSignatureFactory.hpp"

#include "File/FilePath.hpp"

//#include <dxgi1_4.h>
#include <vector>

#include "NextGenGraphics/FrameGraph.hpp"
#include "Vulkan/VulkanHeap.hpp"
#include "Vulkan/VulkanDescriptorHeap.hpp"

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

#include <vector>
#include "NextGenGraphics/FrameGraph.hpp"
#include "Graphics/Format.hpp"
#include "Graphics/View.hpp"
#include "Vulkan/VulkanView.hpp"
#include "Vulkan/VulkanRenderTarget.hpp"
#define PUSHIN(a)	in.push_back((Resource*)(a));
#define PUSHOUT(a)	out.push_back((Resource*)(a));
#define CLEARINOUT()	in.clear(); out.clear();
#include <cstdio>
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Window WindowObj(hInstance, nCmdShow, "Vulkan", "Vulkan", 1280, 720);
	WindowObj.Create(WindowProc);

	//new D3D12ShaderFactory();
	//D3D12Device::Initialize();
	//D3D12Device DeviceObj(0);

	VulkanDevice DeviceObj(WindowObj);
	CommandQueue& DirectCommandQueue = *CreateCommandQueue(DeviceObj, 2);
	SwapChain& SwapChainObj = *CreateSwapChain(DeviceObj, WindowObj, DirectCommandQueue);

	FilePath::Register("..\\eternal-engine-shaders\\Shaders\\", FilePath::SHADERS);

	Viewport ViewportObj(0, 0, 1280, 720);

	//Shader* VS = D3D12ShaderFactory::Get()->CreateVertexShader("PostProcess", "postprocess.vs.hlsl");
	//Shader* PS = D3D12ShaderFactory::Get()->CreatePixelShader("DefaultPostProcess", "defaultpostprocess.ps.hlsl");
	
	VulkanShader VS(DeviceObj, "PostProcessVS", "postprocess.vs.hlsl", VS);
	VulkanShader PS(DeviceObj, "DefaultPostProcessPS", "defaultpostprocess.ps.hlsl", PS);
	
	VulkanHeap HeapTexture(DeviceObj, 1024 * 4, 2, true, false, false, false);
	VulkanHeap HeapStaging(DeviceObj, 1024 * 4, 2, false, true, true, false);

	VulkanResource BufferObj(DeviceObj, HeapTexture, 1024 * 4, (ResourceType)(BUFFER_STRUCTURED_READ | BUFFER_TRANSFER_WRITE));
	VulkanResource StagingObj(DeviceObj, HeapStaging, 1024 * 4, BUFFER_TRANSFER_READ);

	//TextureObj.CreateView(DeviceObj, )

	RootSignature& RootSignatureObj = *CreateRootSignature(DeviceObj);
	VulkanDescriptorHeap DescriptorHeapObj(DeviceObj, STRUCTURED_BUFFER, 2, (RootSignatureAccess)(ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
	DescriptorHeap* DescriptorHeaps[]
	{
		&DescriptorHeapObj
	};
	RootSignature& RootSignatureWithSB = *CreateRootSignature(DeviceObj, nullptr, 0u, DescriptorHeaps, 1, (RootSignatureAccess)(ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));

	View* ViewObj = BufferObj.CreateView(DeviceObj, DescriptorHeapObj, BGRA8888);

	//InputLayout::VertexDataType Vertices[] = {
	//	InputLayout::POSITION_T
	//};
	//D3D12InputLayout InputLayoutObj(Vertices, ETERNAL_ARRAYSIZE(Vertices));
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
	//D3D12InputLayout InputLayoutObj(Vertices, ETERNAL_ARRAYSIZE(Vertices));
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

	//D3D12State StateObj(DeviceObj, InputLayoutObj, VS, PS, DepthTestObj, StencilTestObj, BlendStates, /*SwapChainObj.GetBackBuffer(0),*/ 1, nullptr, 0);

	//D3D12CommandList CommandLists(DeviceObj, DirectCommandQueue, StateObj);
	VulkanCommandList CommandLists[] = {
		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(0)),
		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(1))
	};

	//D3D12Pipeline PipelineObj(DeviceObj, RootSignatureObj, InputLayoutObj, VS, PS, DepthTestObj, StencilTestObj, BlendStates, /*SwapChainObj.GetBackBuffer(0),*/ 1, nullptr, 0);

	Fence& FenceObj = *CreateFence(DeviceObj, 2);
	VulkanPipeline PipelineObj(DeviceObj, RootSignatureWithSB, SwapChainObj.GetMainRenderPass(), VS, PS, ViewportObj);

	VkDescriptorSet DescriptorSets[] =
	{
		static_cast<VulkanDescriptorHeap&>(DescriptorHeapObj).Pop(),
		static_cast<VulkanDescriptorHeap&>(DescriptorHeapObj).Pop()
	};

	static int i = 0;
	for (;;)
	{
		FenceObj.Wait(DeviceObj);
		FenceObj.Reset(DeviceObj);
		uint32_t CurrentFrame = SwapChainObj.AcquireFrame(DeviceObj, FenceObj);
		DirectCommandQueue.Reset(CurrentFrame);
		//CommandLists.Begin(*DirectCommandQueue.GetCommandAllocator(CurrentFrame), PipelineObj);
		//CommandLists.SetViewport(ViewportObj);
		//CommandLists.SetScissorRectangle(ViewportObj);
		//CommandLists.GetD3D12GraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//static_cast<D3D12RenderTarget&>(SwapChainObj.GetBackBuffer(CurrentFrame)).Transition(CommandLists, RENDERTARGET);
		//CommandLists.BindRenderTarget(0, SwapChainObj.GetBackBufferView(CurrentFrame));
		//CommandLists.DrawPrimitive(6);
		//static_cast<D3D12RenderTarget&>(SwapChainObj.GetBackBuffer(CurrentFrame)).Transition(CommandLists, PRESENT);
		//CommandLists.End();

		CommandLists[CurrentFrame].Begin(*DirectCommandQueue.GetCommandAllocator(CurrentFrame), PipelineObj);
		
		void* Data = StagingObj.Map(DeviceObj);
		for (int i = 0; i < 1024; ++i)
		{
			((int*)Data)[i] = 0xFF | (i % 0xFF) << 8 | ((i >> 8) * 0xFF) << 24;
		}
		StagingObj.Unmap(DeviceObj);

		ResourceTransition CopyBufferTransitions[] = {
			ResourceTransition(&StagingObj, TRANSITION_UNDEFINED, TRANSITION_TRANSFER_READ),
			ResourceTransition(&BufferObj, TRANSITION_UNDEFINED, TRANSITION_TRANSFER_WRITE)
		};
		CommandLists[CurrentFrame].Transition(CopyBufferTransitions, ETERNAL_ARRAYSIZE(CopyBufferTransitions), nullptr, 0);
		CommandLists[CurrentFrame].CopyBuffer(StagingObj, BufferObj);

		ResourceTransition UseBufferTransition(&BufferObj, TRANSITION_TRANSFER_WRITE, TRANSITION_SHADER_READ);
		CommandLists[CurrentFrame].Transition(&UseBufferTransition, 1, nullptr, 0);

		CommandLists[CurrentFrame].BeginRenderPass(SwapChainObj.GetMainRenderPass(), SwapChainObj.GetBackBufferView(CurrentFrame).GetAsRenderTarget(), ViewportObj);

#pragma region DescriptorSetStuffTest
		VkDescriptorBufferInfo BufferDescription;
		BufferDescription.buffer = static_cast<VulkanResource&>(BufferObj).GetBuffer();
		BufferDescription.offset = 0;
		BufferDescription.range = 1024 * 4;

		VkWriteDescriptorSet WriteDescriptorSet;
		WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptorSet.pNext = nullptr;
		WriteDescriptorSet.dstSet = DescriptorSets[CurrentFrame];
		WriteDescriptorSet.dstBinding = 0;
		WriteDescriptorSet.dstArrayElement = 0;
		WriteDescriptorSet.descriptorCount = 1;
		WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		WriteDescriptorSet.pImageInfo = nullptr;
		WriteDescriptorSet.pBufferInfo = &BufferDescription;
		WriteDescriptorSet.pTexelBufferView = &static_cast<VulkanView*>(ViewObj)->GetBufferView();

		vkUpdateDescriptorSets(static_cast<VulkanDevice&>(DeviceObj).GetVulkanDevice(), 1, &WriteDescriptorSet, 0, nullptr);

		vkCmdBindDescriptorSets(CommandLists[CurrentFrame].GetVulkanCommandList(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VulkanRootSignature&>(RootSignatureWithSB).GetPipelineLayout(), 0, 1, &DescriptorSets[CurrentFrame], 0, nullptr);
#pragma endregion DescriptorSetStuffTest

		CommandLists[CurrentFrame].DrawPrimitive(6);
		CommandLists[CurrentFrame].EndRenderPass();
		CommandLists[CurrentFrame].End();

		FenceObj.Reset(DeviceObj);
		CommandList* VulkanCommandLists[] = {
			&CommandLists[CurrentFrame]
			//&CommandLists
		};
		DirectCommandQueue.Submit(CurrentFrame, VulkanCommandLists, ETERNAL_ARRAYSIZE(VulkanCommandLists), FenceObj, SwapChainObj);
		FenceObj.Signal(DirectCommandQueue);
		SwapChainObj.Present(DeviceObj, DirectCommandQueue, CurrentFrame);

		MSG Message = { 0 };
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		i++;
	}

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
