#include "Debug.hpp"


using namespace ReShield;
using namespace Eternal::Core;
using namespace Eternal::GameState;

using namespace Eternal::Graphics;
using namespace Eternal::FileSystem;
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


void SampleRenderGeneric(GraphicsContext* Context, Eternal::InputSystem::Input* MultiInputHandle)
{
	Eternal::Parallel::ParallelSystemCreateInformation ParallelCreateInformation;
	Eternal::Parallel::ParallelSystem ParallelSystemDebug(ParallelCreateInformation);

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
