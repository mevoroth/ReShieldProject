#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "Core/Game.hpp"
#include "GameState/CoreState.hpp"
#include "GameState/ReShieldPreGameState.hpp"
#include "Data/ReShieldData.hpp"

#include "Window/Window.hpp"

#include "Graphics/DepthTest.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/ComparisonFunction.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/ShaderType.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/DescriptorTable.hpp"
#include "Graphics/ViewFactory.hpp"
#include "Graphics/ResourceFactory.hpp"

#include "d3d12/D3D12CommandList.hpp"
#include "d3d12/D3D12Resource.hpp"
#include "d3d12/D3D12GraphicsContext.hpp"
#include "d3d12/D3D12Sampler.hpp"
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
#include "Vulkan/VulkanRootSignature.hpp"
#include "Graphics/PipelineFactory.hpp"
#include "Graphics/InputLayoutFactory.hpp"
#include "Graphics/RenderPassFactory.hpp"
#include "Graphics/DepthStencil.hpp"
#include "Graphics/SamplerFactory.hpp"
#include "Resources/TextureFactory.hpp"
#include "Resources/ImmediateTextureFactory.hpp"
#include "Import/tga/ImportTga.hpp"
#include "DebugTools/Debug.hpp"
#include "Imgui/Imgui.hpp"
#include "Input/InputFactory.hpp"
#include "Platform/WindowsProcess.hpp"

using namespace ReShield;
using namespace Eternal::Core;
using namespace Eternal::GameState;

using namespace Eternal::Graphics;
using namespace Eternal::File;
using namespace std;

static bool IsRunning = true;

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
		IsRunning = false;
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#include "Vulkan/VulkanHeader.hpp"
#include "NextGenGraphics/FrameGraph.hpp"
#include "Graphics/Format.hpp"
#include "Graphics/View.hpp"
#include "Vulkan/VulkanUtils.hpp"
#include "Vulkan/VulkanView.hpp"
#include "Vulkan/VulkanResource.hpp"
#define PUSHIN(a)	in.push_back((Resource*)(a));
#define PUSHOUT(a)	out.push_back((Resource*)(a));
#define CLEARINOUT()	in.clear(); out.clear();
#include <cstdio>
#include "Graphics/Types.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Time/TimeFactory.hpp"
#include "Time/Time.hpp"
#include "Log/LogFactory.hpp"
#include "Log/Log.hpp"
#include "Optick/Optick.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include "optick.h"

void SampleRenderGeneric(GraphicsContext* Context, Eternal::Input::Input* MultiInputHandle)
{
	OPTICK_THREAD("MainThread");
	Eternal::Imgui::Imgui Imgui(*Context, MultiInputHandle);

	ImmediateTextureFactoryLoadTextureCallback ImmediateLoadTexture;
	ImmediateTextureFactoryCreateGpuResourceCallback ImmediateCreateGpuResource(*Context);

	TextureFactoryCreateInformation TexFactoryCreateInformation =
	{
		ImmediateLoadTexture,
		ImmediateCreateGpuResource
	};
	TextureFactory TexFactory(TexFactoryCreateInformation);

	TextureFactoryRequest Request("Noise", ".\\noise.tga");
	TexFactory.CreateRequest(Request);

	Shader& SampleTexturePS = *Context->GetShader(ShaderCreateInformation(ShaderType::PS, "SampleTexture", "sampletexture.ps.hlsl"));
	Shader& VS = *Context->GetShader(ShaderCreateInformation(ShaderType::VS, "PostProcess", "postprocess.vs.hlsl"));
	Shader& RayMarchingPS = *Context->GetShader(ShaderCreateInformation(ShaderType::PS, "RayMarch_00", "raymarching_00.ps.hlsl"));

	SamplerCreateInformation SamplerInformation;
	Sampler* BilinearSampler = CreateSampler(*Context, SamplerInformation);

	RootSignatureCreateInformation RootSignatureInformation(
		{
			RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
		}
	);

	RootSignature* DefaultRootSignature = CreateRootSignature(*Context, RootSignatureInformation);
	InputLayout* DefaultInputLayout = CreateInputLayout(*Context);

	vector<View*>& BackBufferViews = Context->GetSwapChain().GetBackBufferRenderTargetViews();

	vector<RenderPass*> RenderPasses;
	RenderPasses.resize(BackBufferViews.size());

	for (int RenderPassIndex = 0; RenderPassIndex < BackBufferViews.size(); ++RenderPassIndex)
	{
		vector<RenderTargetInformation> CurrentRenderTargets = {
			RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, BackBufferViews[RenderPassIndex])
		};

		RenderPassCreateInformation RenderPassInformation(
			Context->GetMainViewport(),
			CurrentRenderTargets
		);

		RenderPasses[RenderPassIndex] = CreateRenderPass(*Context, RenderPassInformation);
	}

	PipelineCreateInformation PipelineInformation(
		*DefaultRootSignature,
		*DefaultInputLayout,
		*RenderPasses[0],
		VS,
		//RayMarchingPS,
		SampleTexturePS
	);

	Pipeline* RayMarchingPipeline = CreatePipeline(*Context, PipelineInformation);
	
	struct FrameConstants
	{
		float Multiplier;
		float Offset;
		float _Pad0;
		float _Pad1;
	};

	BufferResourceCreateInformation FrameConstantBufferCreateInformation(
		Context->GetDevice(),
		"SamplerTextureConstantBuffer",
		BufferCreateInformation(
			Format::FORMAT_UNKNOWN,
			BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
			sizeof(FrameConstants)
		),
		ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
	);
	Resource* ConstantBuffer = CreateBuffer(FrameConstantBufferCreateInformation);
	ViewMetaData FrameConstantBufferMetaData;
	FrameConstantBufferMetaData.ConstantBufferView.BufferSize = sizeof(FrameConstants);
	ConstantBufferViewCreateInformation FrameConstantBufferViewCreateInformation(
		*Context,
		ConstantBuffer,
		FrameConstantBufferMetaData
	);
	View* ConstantBufferView = CreateConstantBufferView(FrameConstantBufferViewCreateInformation);

	DescriptorTable* SampleTextureDescriptorTable = DefaultRootSignature->CreateRootDescriptorTable(*Context);

	View* NoiseTextureView = nullptr;

	int FrameIndex = 0;
	while (IsRunning)
	{
		OPTICK_FRAME("EternalFrame");

		{
			OPTICK_EVENT("BeginFrame");
			Eternal::Platform::WindowsProcess::ExecuteMessageLoop();
		}

		{
			OPTICK_EVENT("InputUpdate");
			MultiInputHandle->Update();
		}

		{
			OPTICK_EVENT("BeginFrame");
			Context->BeginFrame();
		}

		{
			OPTICK_EVENT("TextureStreaming");
			TexFactory.ProcessRequests();
		}

		{
			OPTICK_EVENT("Imgui");
			Imgui.Begin();

			bool IsWindowOpen = true;
			ImGui::ShowMetricsWindow(&IsWindowOpen);

			ImGui::Begin("Debug");
			ImGui::Text("Frame Index: [%d]", FrameIndex++);
			ImGui::End();

			Imgui.End();
		}

		{
			OPTICK_EVENT("TransitionToRenderTarget");
			CommandList* TransitionToRenderTargetCommandList = Context->CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "TransitionToRenderTarget");
			TransitionToRenderTargetCommandList->Begin(*Context);
			ResourceTransition BackBufferPresentToRenderTarget(BackBufferViews[Context->GetCurrentFrameIndex()], TransitionState::TRANSITION_RENDER_TARGET);
			TransitionToRenderTargetCommandList->Transition(&BackBufferPresentToRenderTarget, 1);
			TransitionToRenderTargetCommandList->End();
		}

		{
			OPTICK_EVENT("SetResourceTable");
			Resource* NoiseTexture = TexFactory.GetTexture("Noise");
			if (!NoiseTextureView)
			{
				ViewMetaData NoiseViewMetaData;
				NoiseViewMetaData.ShaderResourceViewTexture2D.MipLevels = 1;
				ShaderResourceViewCreateInformation NoiseTextureShaderResourceViewCreateInformation(
					*Context,
					NoiseTexture,
					NoiseViewMetaData,
					Format::FORMAT_BGRA8888_UNORM,
					ViewShaderResourceType::VIEW_SHADER_RESOURCE_TEXTURE_2D
				);

				NoiseTextureView = CreateShaderResourceView(NoiseTextureShaderResourceViewCreateInformation);
			}

			MapRange ConstantBufferMapRange(sizeof(FrameConstants));
			FrameConstants* FrameConstantsPtr = static_cast<FrameConstants*>(ConstantBuffer->Map(ConstantBufferMapRange));
			FrameConstantsPtr->Multiplier = 2;
			FrameConstantsPtr->Offset = -1;
			ConstantBuffer->Unmap(ConstantBufferMapRange);

			SampleTextureDescriptorTable->SetDescriptor(0, ConstantBufferView);
			SampleTextureDescriptorTable->SetDescriptor(1, NoiseTextureView);
			SampleTextureDescriptorTable->SetDescriptor(2, NoiseTextureView);
			SampleTextureDescriptorTable->SetDescriptor(3, BilinearSampler);
		}
		{
			OPTICK_EVENT("DrawSample");
			CommandList* CurrentCommandList = Context->CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "DrawSample");

			CurrentCommandList->Begin(*Context);
			CurrentCommandList->BeginRenderPass(*RenderPasses[Context->GetCurrentFrameIndex()]);

			CurrentCommandList->SetGraphicsPipeline(*RayMarchingPipeline);
			{
				OPTICK_EVENT("SetGraphicsResourceTable");
				CurrentCommandList->SetGraphicsDescriptorTable(*Context, *SampleTextureDescriptorTable);
			}

			CurrentCommandList->DrawInstanced(6);

			CurrentCommandList->EndRenderPass();
			CurrentCommandList->End();
		}
		{
			OPTICK_EVENT("ImguiRender");
			Imgui.Render(*Context);
		}
		{
			OPTICK_EVENT("TransitionToPresent");
			CommandList* TransitionToBackBufferCommandList = Context->CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "TransitionToPresent");
			ResourceTransition BackBufferRenderTargetToPresent(BackBufferViews[Context->GetCurrentFrameIndex()], TransitionState::TRANSITION_PRESENT);
			TransitionToBackBufferCommandList->Begin(*Context);
			TransitionToBackBufferCommandList->Transition(&BackBufferRenderTargetToPresent, 1);
			TransitionToBackBufferCommandList->End();
		}
		{
			OPTICK_EVENT("EndFrame");
			Context->EndFrame();
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Eternal::DebugTools::WaitForDebugger();

	const DeviceType ProgramDeviceType = DeviceType::VULKAN;

	const char* AppName = ProgramDeviceType == DeviceType::D3D12 ? "D3D12" : "Vulkan";

	Eternal::Time::Time* Timer = Eternal::Time::CreateTime(Eternal::Time::TimeType::WIN);
	Eternal::Log::Log* ConsoleLog = Eternal::Log::CreateLog(Eternal::Log::CONSOLE, "Eternal");
	Eternal::Log::Log::Initialize(ConsoleLog);

	Eternal::Platform::WindowsProcess Process;

	Eternal::Input::Input* MultiInputHandle = Eternal::Input::CreateMultiInput({
		Eternal::Input::InputType::WIN,
		Eternal::Input::InputType::XINPUT
	});

	Eternal::Platform::WindowsProcess::SetInputHandler(MultiInputHandle);
	Eternal::Platform::WindowsProcess::SetIsRunning(&IsRunning);

	using namespace Eternal::Graphics;
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
		Eternal::Platform::WindowsProcess::WindowProc
	);
	GraphicsContextCreateInformation ContextCreateInformation(Settings, WinArguments);

	FilePath::Register("..\\eternal-engine-shaders\\Shaders\\", FileType::SHADERS);
	FilePath::Register("..\\assets\\textures\\", FileType::TEXTURES);

	ImportTga* TgaImporter = new ImportTga();

	GraphicsContext* Context = CreateGraphicsContext(ContextCreateInformation);
	
	OPTICK_APP(AppName);
	Eternal::Optick::OptickStart(*Context);
	//SampleRender(Context, Timer);
	SampleRenderGeneric(Context, MultiInputHandle);
	Eternal::Optick::OptickStop();

	DestroyGraphicsContext(Context);
	
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

	//delete ReShieldPreGameStateObj;
	//ReShieldPreGameStateObj = nullptr;

	return 0;
}
