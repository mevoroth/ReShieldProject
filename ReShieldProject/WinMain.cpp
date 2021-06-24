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
#include "Graphics/Comparison.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/ShaderType.hpp"
#include "Graphics/ShaderFactory.hpp"

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
#include "Vulkan_deprecated/VulkanRootSignature.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
//*/
#include "Graphics/CommandQueueFactory.hpp"
#include "Graphics/SwapChainFactory.hpp"
#include "Graphics/FenceFactory.hpp"
#include "Graphics/Fence.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/SwapChain.hpp"
#include "Graphics_deprecated/RootSignature.hpp"
#include "Graphics_deprecated/RootSignatureFactory.hpp"

#include "File/FilePath.hpp"

//#include <dxgi1_4.h>
#include <vector>

#include "NextGenGraphics/FrameGraph.hpp"
#include "Vulkan_deprecated/VulkanHeap.hpp"
#include "Vulkan_deprecated/VulkanDescriptorHeap.hpp"
#include "Vulkan_deprecated/VulkanRootSignature.hpp"
#include "Graphics_deprecated/PipelineFactory.hpp"
#include "Graphics_deprecated/InputLayoutFactory.hpp"
#include "Graphics/RenderPassFactory.hpp"
#include "Graphics/DepthStencil.hpp"
#include "Resources/TextureFactory.hpp"
#include "Resources/ImmediateTextureFactory.hpp"
#include "Import/tga/ImportTga.hpp"
#include "DebugTools/Debug.hpp"

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

#include "Vulkan/VulkanHeader.hpp"
#include "NextGenGraphics/FrameGraph.hpp"
#include "Graphics/Format.hpp"
#include "Graphics/View.hpp"
#include "Vulkan/VulkanUtils.hpp"
#include "Vulkan/VulkanView.hpp"
#include "Vulkan_deprecated/VulkanRenderTarget.hpp"
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

void SampleRender(GraphicsContext* Context, Eternal::Time::Time* Timer)
{
	ImmediateTextureFactoryLoadTextureCallback ImmediateLoadTexture;

	class DebugVulkanCreateGpuResource : public TextureFactoryCreateGpuResourceCallback
	{
	public:
		GraphicsContext& Context;
		DebugVulkanCreateGpuResource(GraphicsContext& InContext)
			: Context(InContext)
		{
		}

		virtual bool CreateTexture(_Inout_ RawTextureData& TextureData, _Out_ Texture*& OutTexture) override
		{
			//////////////////////////////////////////////////////////////////////////
			// CPU Buffer
			vk::Buffer			StagingBuffer;
			vk::DeviceMemory	StagingBufferMemory;

			vk::BufferCreateInfo BufferInfo(
				vk::BufferCreateFlagBits(),
				TextureData.Width * TextureData.Height * 4,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::SharingMode::eExclusive
			);

			vk::Device& VulkanDeviceObj = static_cast<VulkanDevice&>(Context.GetDevice()).GetVulkanDevice();
			Vulkan::VerifySuccess(VulkanDeviceObj.createBuffer(&BufferInfo, nullptr, &StagingBuffer));

			GraphicsMemoryFlag MemoryFlag = GraphicsMemoryFlag::MEMORY_FLAG_MAPPABLE | GraphicsMemoryFlag::MEMORY_FLAG_MANAGED_CACHE;

			vk::MemoryRequirements StagingBufferMemoryRequirements = VulkanDeviceObj.getBufferMemoryRequirements(StagingBuffer);

			vk::MemoryAllocateInfo StagingBufferMemoryAllocateInfo(
				StagingBufferMemoryRequirements.size,
				static_cast<VulkanDevice&>(Context.GetDevice()).FindBestMemoryTypeIndex(
					uint32_t(StagingBufferMemoryRequirements.memoryTypeBits),
					Vulkan::ConvertGraphicsMemoryFlagsToMemoryPropertyFlags(MemoryFlag)
				)
			);

			Vulkan::VerifySuccess(VulkanDeviceObj.allocateMemory(&StagingBufferMemoryAllocateInfo, nullptr, &StagingBufferMemory));
			VulkanDeviceObj.bindBufferMemory(StagingBuffer, StagingBufferMemory, 0);

			void* StagingData = nullptr;
			Vulkan::VerifySuccess(VulkanDeviceObj.mapMemory(StagingBufferMemory, 0, StagingBufferMemoryRequirements.size, vk::MemoryMapFlagBits(), &StagingData));
			memcpy(StagingData, TextureData.TextureData, TextureData.Width * TextureData.Height * 4);
			VulkanDeviceObj.unmapMemory(StagingBufferMemory);

			//////////////////////////////////////////////////////////////////////////
			// GPU
			vk::Image& Texture = *new vk::Image;
			vk::DeviceMemory TextureMemory;

			vk::ImageCreateInfo TextureInfo(
				vk::ImageCreateFlagBits(),
				vk::ImageType::e2D,
				vk::Format::eR8G8B8A8Unorm,
				vk::Extent3D(TextureData.Width, TextureData.Height, 1),
				1, 1,
				vk::SampleCountFlagBits::e1,
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::SharingMode::eExclusive,
				0, nullptr,
				vk::ImageLayout::eUndefined
			);

			Vulkan::VerifySuccess(VulkanDeviceObj.createImage(&TextureInfo, nullptr, &Texture));
			vk::MemoryRequirements TextureMemoryRequirements = VulkanDeviceObj.getImageMemoryRequirements(Texture);

			MemoryFlag = GraphicsMemoryFlag::MEMORY_FLAG_GPU;
			vk::MemoryAllocateInfo TextureMemoryAllocateInfo(
				TextureMemoryRequirements.size,
				static_cast<VulkanDevice&>(Context.GetDevice()).FindBestMemoryTypeIndex(
					uint32_t(StagingBufferMemoryRequirements.memoryTypeBits),
					Vulkan::ConvertGraphicsMemoryFlagsToMemoryPropertyFlags(MemoryFlag)
				)
			);

			Vulkan::VerifySuccess(VulkanDeviceObj.allocateMemory(&TextureMemoryAllocateInfo, nullptr, &TextureMemory));
			VulkanDeviceObj.bindImageMemory(Texture, TextureMemory, 0);

			vk::ImageMemoryBarrier TextureMemoryBarrier(
				vk::AccessFlagBits(),
				vk::AccessFlagBits::eTransferWrite,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				0, 0,
				Texture,
				vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eColor,
					0, 1,
					0, 1
				)
			);

			CmdList.pipelineBarrier(
				vk::PipelineStageFlagBits::eHost,
				vk::PipelineStageFlagBits::eTransfer,
				vk::DependencyFlagBits(),
				0, static_cast<vk::MemoryBarrier*>(nullptr),
				0, static_cast<vk::BufferMemoryBarrier*>(nullptr),
				1, &TextureMemoryBarrier
			);

			vk::BufferImageCopy TextureRegion(
				0ull, TextureData.Width, TextureData.Height,
				vk::ImageSubresourceLayers(
					vk::ImageAspectFlagBits::eColor,
					0, 0, 1
				),
				vk::Offset3D(),
				vk::Extent3D(TextureData.Width, TextureData.Height, TextureData.Depth)
			);

			CmdList.copyBufferToImage(
				StagingBuffer,
				Texture,
				vk::ImageLayout::eTransferDstOptimal,
				1, &TextureRegion
			);

			TextureMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
			TextureMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
			TextureMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
			TextureMemoryBarrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

			CmdList.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eFragmentShader,
				vk::DependencyFlagBits(),
				0, static_cast<vk::MemoryBarrier*>(nullptr),
				0, static_cast<vk::BufferMemoryBarrier*>(nullptr),
				1, &TextureMemoryBarrier
			);

			//// Clean up staging resources
			//vkFreeMemory(get_device().get_handle(), staging_memory, nullptr);
			//vkDestroyBuffer(get_device().get_handle(), staging_buffer, nullptr);

			TextureData.Release();

			OutTexture = reinterpret_cast<Eternal::Graphics::Texture*>(&Texture);

			return true;
		}

		void HACKSetCommandList(const vk::CommandBuffer& InCmdList)
		{
			CmdList = InCmdList;
		}

		vk::CommandBuffer CmdList;
	} DebugVulkanCreateGpuResourceCB(*Context);

	TextureFactoryCreateInformation TexFactoryCreateInformation =
	{
		ImmediateLoadTexture,
		DebugVulkanCreateGpuResourceCB
	};
	TextureFactory TexFactory(TexFactoryCreateInformation);

	VulkanShader VS(
		*Context,
		ShaderCreateInformation(ShaderType::VS, "PostProcess", "postprocess.vs.hlsl")
	);
	//VulkanShader PS(
	//	*Context,
	//	ShaderCreateInformation(ShaderType::PS, "DefaultPostProcess", "defaultpostprocess.ps.hlsl")
	//);
	VulkanShader PS(
		*Context,
		ShaderCreateInformation(ShaderType::PS, "RayMarch_00", "raymarching_00.ps.hlsl")
	);

	vector<RootSignatureParameter> Parameters = {
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_DYNAMIC_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 0 },
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 1 },
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 2 }
	};

	RootSignature* DefaultRootSignature = CreateRootSignature(*Context, &Parameters, 1, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS);
	InputLayout* DefaultInputLayout = CreateInputLayout(*Context);

	const vector<View*>& BackBufferViews = Context->GetSwapChain().GetBackBufferRenderTargetViews();

	vector<RenderPass*> RenderPasses;
	RenderPasses.resize(BackBufferViews.size());

	for (int RenderPassIndex = 0; RenderPassIndex < BackBufferViews.size(); ++RenderPassIndex)
	{
		vector<RenderTargetInformation> CurrentRenderTargets = {
			RenderTargetInformation(BlendStateNone, RenderTargetOperator::NoLoad_Store, BackBufferViews[RenderPassIndex])
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
		VS, PS,
		DepthStencilNoneNone
	);

	Pipeline* TestPipeline = CreatePipeline(*Context, PipelineInformation);

	vk::Device& DeviceObj = static_cast<VulkanDevice&>(Context->GetDevice()).GetVulkanDevice();
	vk::PhysicalDevice& PhysDevice = static_cast<VulkanDevice&>(Context->GetDevice()).GetPhysicalDevice();
	vk::SurfaceKHR& SwapChainSurface = static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetSurface();
	vk::SwapchainKHR& SwapChainObj = static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetSwapChain();

	vk::Queue VulkanQueue;
	DeviceObj.getQueue(0, 0, &VulkanQueue);

	vk::FenceCreateInfo FenceInfo;
	vk::Fence SubmitFence;
	Vulkan::VerifySuccess(DeviceObj.createFence(&FenceInfo, nullptr, &SubmitFence));

	vk::CommandPoolCreateInfo CommandPoolInfo(
		vk::CommandPoolCreateFlagBits::eTransient, 0
	);
	vk::CommandPool VulkanCommandPool;
	Vulkan::VerifySuccess(DeviceObj.createCommandPool(&CommandPoolInfo, nullptr, &VulkanCommandPool));

	vk::CommandBufferAllocateInfo CommandBufferInfo(
		VulkanCommandPool,
		vk::CommandBufferLevel::ePrimary,
		2
	);
	vk::CommandBuffer CommandBuffers[2];
	Vulkan::VerifySuccess(DeviceObj.allocateCommandBuffers(&CommandBufferInfo, CommandBuffers));
	vk::CommandBuffer& VulkanCommandBuffer = CommandBuffers[0];

	std::vector<vk::Semaphore> AcquireSemaphores;
	vk::SemaphoreCreateInfo SemaphoreInfo;
	AcquireSemaphores.resize(2);
	for (int SemIndex = 0; SemIndex < 2; ++SemIndex)
	{
		Vulkan::VerifySuccess(DeviceObj.createSemaphore(&SemaphoreInfo, nullptr, &AcquireSemaphores[SemIndex]));
	}

	vk::Semaphore ReleaseSemaphore;
	Vulkan::VerifySuccess(DeviceObj.createSemaphore(&SemaphoreInfo, nullptr, &ReleaseSemaphore));

	struct PerFrame
	{
		float ElapsedTime;
		float _Pad0;
		float _Pad1;
		float _Pad2;
	};

	vk::BufferCreateInfo BufferInfo(
		vk::BufferCreateFlagBits(),
		sizeof(PerFrame),
		vk::BufferUsageFlagBits::eUniformBuffer,
		vk::SharingMode::eExclusive
	);

	vk::Buffer PerFrameConstantBuffer;
	Vulkan::VerifySuccess(DeviceObj.createBuffer(&BufferInfo, nullptr, &PerFrameConstantBuffer));

	GraphicsMemoryFlag MemoryFlag = GraphicsMemoryFlag::MEMORY_FLAG_MAPPABLE;

	vk::MemoryRequirements BufferMemoryRequirements = DeviceObj.getBufferMemoryRequirements(PerFrameConstantBuffer);
	vk::MemoryAllocateInfo MemoryInfo(
		sizeof(PerFrame),
		static_cast<VulkanDevice&>(Context->GetDevice()).FindBestMemoryTypeIndex(
			uint32_t(BufferMemoryRequirements.memoryTypeBits),
			Vulkan::ConvertGraphicsMemoryFlagsToMemoryPropertyFlags(MemoryFlag)
		)
	);

	const uint32_t BufferSize = BufferMemoryRequirements.size;

	vk::DeviceMemory BufferMemory;
	Vulkan::VerifySuccess(DeviceObj.allocateMemory(&MemoryInfo, nullptr, &BufferMemory));

	std::array<vk::DescriptorPoolSize, 3> PoolSizes;
	PoolSizes[0] = vk::DescriptorPoolSize(
		vk::DescriptorType::eUniformBufferDynamic,
		16
	);
	PoolSizes[1] = vk::DescriptorPoolSize(
		vk::DescriptorType::eSampledImage,
		16
	);
	PoolSizes[2] = vk::DescriptorPoolSize(
		vk::DescriptorType::eSampler,
		16
	);

	vk::DescriptorPoolCreateInfo DescriptorPoolInfo(
		vk::DescriptorPoolCreateFlagBits(),
		2,
		PoolSizes.size(), PoolSizes.data()
	);
	vk::DescriptorPool DescPool;
	Vulkan::VerifySuccess(DeviceObj.createDescriptorPool(&DescriptorPoolInfo, nullptr, &DescPool));

	const vk::DescriptorSetLayout& PipeLayout = ((VulkanRootSignature*)DefaultRootSignature)->GetDescriptorSetLayouts()[0];

	vk::DescriptorSetAllocateInfo DescriptorSetInfo(
		DescPool,
		1, &PipeLayout
	);
	vk::DescriptorSet FrameConstantsDescriptorSet;
	DeviceObj.allocateDescriptorSets(
		&DescriptorSetInfo,
		&FrameConstantsDescriptorSet
	);

	vk::ImageView NoiseTextureView;

	vk::Sampler NoiseTextureSampler;
	vk::SamplerCreateInfo NoiseTextureSamplerCreateInfo(
		vk::SamplerCreateFlagBits(),
		vk::Filter::eLinear,
		vk::Filter::eLinear,
		vk::SamplerMipmapMode::eLinear
	);
	Vulkan::VerifySuccess(DeviceObj.createSampler(
		&NoiseTextureSamplerCreateInfo,
		nullptr,
		&NoiseTextureSampler
	));

	TextureFactoryRequest Request("Noise", ".\\noise.tga");
	TexFactory.CreateRequest(Request);

	double ElapsedTime = 0.0;

	static int i = 0;
	for (;;)
	{
		Timer->Update();
		ElapsedTime += Timer->GetDeltaTimeSeconds();

		vk::CommandBufferBeginInfo CommandBufferBegin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		bool HasRequests = TexFactory.HasRequests();

		DebugVulkanCreateGpuResourceCB.HACKSetCommandList(CommandBuffers[1]);

		if (HasRequests)
			CommandBuffers[1].begin(&CommandBufferBegin);
		TexFactory.ProcessRequests();
		if (HasRequests)
			CommandBuffers[1].end();

		uint32_t ImageIndex;
		vk::Semaphore& CurrentSemaphore = AcquireSemaphores[i % 2];

		Vulkan::VerifySuccess(DeviceObj.acquireNextImageKHR(SwapChainObj, UINT64_MAX, CurrentSemaphore, nullptr, &ImageIndex));

		if (i != 0)
		{
			DeviceObj.waitForFences(1, &SubmitFence, true, UINT64_MAX);
			DeviceObj.resetFences(1, &SubmitFence);
		}

		DeviceObj.resetCommandPool(VulkanCommandPool, vk::CommandPoolResetFlagBits());

		vk::RenderPassBeginInfo BeginRenderPassInfo(
			static_cast<VulkanRenderPass&>(*RenderPasses[i % 2]).GetVulkanRenderPass(),
			static_cast<VulkanRenderPass&>(*RenderPasses[i % 2]).GetFrameBuffer(),
			Vulkan::ConvertViewportToRect2D(Context->GetMainViewport())
		);

		Vulkan::VerifySuccess(VulkanCommandBuffer.begin(&CommandBufferBegin));

		const vk::Image& BackBufferImage = static_cast<VulkanResource*>(Context->GetSwapChain().GetBackBuffers()[ImageIndex])->GetVulkanImage();

		vk::ImageMemoryBarrier PresentToColorAttachmentOutput(
			vk::AccessFlagBits::eColorAttachmentRead,
			vk::AccessFlagBits::eColorAttachmentWrite,
			i <= 1 ? vk::ImageLayout::eUndefined : vk::ImageLayout::ePresentSrcKHR,
			vk::ImageLayout::eColorAttachmentOptimal,
			0, 0,
			BackBufferImage,
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0, 1,
				0, 1
			)
		);

		PerFrame* PerFrameData;
		DeviceObj.mapMemory(BufferMemory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlagBits(), reinterpret_cast<void**>(&PerFrameData));
		PerFrameData->ElapsedTime = (float)ElapsedTime;
		DeviceObj.unmapMemory(BufferMemory);
		if (i == 0)
			DeviceObj.bindBufferMemory(PerFrameConstantBuffer, BufferMemory, 0ull);

		std::array<vk::WriteDescriptorSet, 2> NewDescriptorSets;

		vk::DeviceSize Range = sizeof(PerFrame);
		vk::DescriptorBufferInfo BufferDescriptorInfo(
			PerFrameConstantBuffer,
			0, Range
		);
		NewDescriptorSets[0] = vk::WriteDescriptorSet(
			FrameConstantsDescriptorSet,
			0, 0, 1, vk::DescriptorType::eUniformBufferDynamic,
			nullptr, &BufferDescriptorInfo, nullptr
		);

		vk::Image* NoiseTexture = reinterpret_cast<vk::Image*>(TexFactory.GetTexture("Noise"));
		if (NoiseTexture && !NoiseTextureView)
		{
			vk::ImageViewCreateInfo NoiseImageViewInformation(
				vk::ImageViewCreateFlagBits(),
				*NoiseTexture,
				vk::ImageViewType::e2D,
				vk::Format::eR8G8B8A8Unorm,
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eColor,
					0, 1, 0, 1
				)
			);
			Vulkan::VerifySuccess(DeviceObj.createImageView(&NoiseImageViewInformation, nullptr, &NoiseTextureView));
		}

		vk::DescriptorImageInfo ImageDescriptorInfo(
			vk::Sampler(),
			NoiseTextureView,
			vk::ImageLayout::eShaderReadOnlyOptimal
		);

		NewDescriptorSets[1] = vk::WriteDescriptorSet(
			FrameConstantsDescriptorSet,
			1, 0, 1, vk::DescriptorType::eSampledImage,
			&ImageDescriptorInfo, nullptr, nullptr
		);

		DeviceObj.updateDescriptorSets(
			NewDescriptorSets.size(), NewDescriptorSets.data(),
			0, nullptr
		);

		//for (int32_t NewDescriptorSetIndex = 0; NewDescriptorSetIndex < NewDescriptorSets.size(); ++NewDescriptorSetIndex)
		//{
		//	DeviceObj.updateDescriptorSets(
		//		1, &NewDescriptorSets[NewDescriptorSetIndex],
		//		0, nullptr
		//	);
		//}

		VulkanCommandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllGraphics,
			vk::PipelineStageFlagBits::eAllGraphics,
			vk::DependencyFlagBits(),
			0, static_cast<vk::MemoryBarrier*>(nullptr),
			0, static_cast<vk::BufferMemoryBarrier*>(nullptr),
			1, &PresentToColorAttachmentOutput
		);

		uint32_t DynamicOffset = 0;

		VulkanCommandBuffer.beginRenderPass(&BeginRenderPassInfo, vk::SubpassContents::eInline);
		VulkanCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, static_cast<VulkanPipeline&>(*TestPipeline).GetVulkanPipeline());
		VulkanCommandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			((VulkanRootSignature*)DefaultRootSignature)->GetPipelineLayout(),
			0,
			0, nullptr,
			//1, FrameConstantsDescriptorSet,
			//1, &DynamicOffset
			0, nullptr
		);
		VulkanCommandBuffer.draw(6, 1, 0, 0);
		VulkanCommandBuffer.endRenderPass();

		vk::ImageMemoryBarrier ColorAttachmentOutputToPresent(
			vk::AccessFlagBits::eColorAttachmentWrite,
			vk::AccessFlagBits::eColorAttachmentRead,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			0, 0,
			BackBufferImage,
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0, 1,
				0, 1
			)
		);

		VulkanCommandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllGraphics,
			vk::PipelineStageFlagBits::eAllGraphics,
			vk::DependencyFlagBits(),
			0, static_cast<vk::MemoryBarrier*>(nullptr),
			0, static_cast<vk::BufferMemoryBarrier*>(nullptr),
			1, &ColorAttachmentOutputToPresent
		);

		VulkanCommandBuffer.end();

		vk::CommandBuffer SubmitCmdList[2] = {
			CommandBuffers[1],
			CommandBuffers[0]
		};

		vk::PipelineStageFlags WaitDestStageMask = vk::PipelineStageFlagBits::eAllGraphics;
		vk::SubmitInfo SubmitInfo(
			1, &CurrentSemaphore,
			&WaitDestStageMask,
			HasRequests ? 2 : 1, HasRequests ? SubmitCmdList : &VulkanCommandBuffer,
			1, &ReleaseSemaphore
		);
		Vulkan::VerifySuccess(VulkanQueue.submit(1, &SubmitInfo, SubmitFence));

		//present
		vk::PresentInfoKHR PresentInfo(
			1, &ReleaseSemaphore,
			1, &SwapChainObj,
			&ImageIndex
		);
		Vulkan::VerifySuccess(VulkanQueue.presentKHR(&PresentInfo));

		MSG Message = { 0 };
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		i++;
	}
}

void SampleRenderGeneric(GraphicsContext* Context)
{
	Shader& VS = *Context->GetShaderFactory().GetShader(*Context, ShaderCreateInformation(ShaderType::VS, "PostProcess", "postprocess.vs.hlsl"));
	Shader& PS = *Context->GetShaderFactory().GetShader(*Context, ShaderCreateInformation(ShaderType::PS, "RayMarch_00", "raymarching_00.ps.hlsl"));

	vector<RootSignatureParameter> Parameters = {
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_DYNAMIC_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 0 },
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 1 },
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 2 }
	};

	RootSignature* DefaultRootSignature = CreateRootSignature(*Context, &Parameters, 1, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS);
	InputLayout* DefaultInputLayout = CreateInputLayout(*Context);

	const vector<View*>& BackBufferViews = Context->GetSwapChain().GetBackBufferRenderTargetViews();

	vector<RenderPass*> RenderPasses;
	RenderPasses.resize(BackBufferViews.size());

	for (int RenderPassIndex = 0; RenderPassIndex < BackBufferViews.size(); ++RenderPassIndex)
	{
		vector<RenderTargetInformation> CurrentRenderTargets = {
			RenderTargetInformation(BlendStateNone, RenderTargetOperator::NoLoad_Store, BackBufferViews[RenderPassIndex])
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
		VS, PS,
		DepthStencilNoneNone
	);

	Pipeline* RayMarchingPipeline = CreatePipeline(*Context, PipelineInformation);

	for (;;)
	{
		Context->GetSubmitFence().Wait(Context->GetDevice());
		Context->GetSubmitFence().Reset(Context->GetDevice());

		Context->GetSwapChain().Acquire(*Context);

		CommandList* CurrentCommandList = Context->CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC);

		CurrentCommandList->Begin();
		CurrentCommandList->End();

		Context->GetGraphicsQueue().SubmitCommandLists(
			*Context,
			&CurrentCommandList,
			1
		);

		Context->GetSwapChain().Present(*Context);

		delete CurrentCommandList; // Needs defer delete
		
		MSG Message = { 0 };
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Eternal::DebugTools::WaitForDebugger();

	Eternal::Time::Time* Timer = Eternal::Time::CreateTime(Eternal::Time::TimeType::WIN);
	Eternal::Log::Log* ConsoleLog = Eternal::Log::CreateLog(Eternal::Log::CONSOLE, "Eternal");
	Eternal::Log::Log::Initialize(ConsoleLog);

	using namespace Eternal::Graphics;
	RenderSettings Settings(1600, 900, DeviceType::VULKAN);
	WindowsArguments WinArguments(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Vulkan", "Vulkan", WindowProc);
	GraphicsContextCreateInformation ContextCreateInformation(Settings, WinArguments);

	FilePath::Register("..\\eternal-engine-shaders\\Shaders\\", FileType::SHADERS);
	FilePath::Register("..\\assets\\textures\\", FileType::TEXTURES);

	ImportTga* TgaImporter = new ImportTga();

	GraphicsContext* Context = CreateGraphicsContext(ContextCreateInformation);

	//SampleRender(Context, Timer);
	SampleRenderGeneric(Context);

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
