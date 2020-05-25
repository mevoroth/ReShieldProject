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
#include "Vulkan/VulkanRootSignature.hpp"
#include "Graphics/PipelineFactory.hpp"
#include "Graphics/InputLayoutFactory.hpp"
#include "Graphics/RenderPassFactory.hpp"
#include "Graphics/DepthStencil.hpp"
#include "Resources/TextureFactory.hpp"
#include "Resources/ImmediateTextureFactory.hpp"
#include "Import/tga/ImportTga.hpp"

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

#include <vulkan/vulkan.hpp>
#include "NextGenGraphics/FrameGraph.hpp"
#include "Graphics/Format.hpp"
#include "Graphics/View.hpp"
#include "Vulkan/VulkanUtils.hpp"
#include "Vulkan/VulkanView.hpp"
#include "Vulkan/VulkanRenderTarget.hpp"
#define PUSHIN(a)	in.push_back((Resource*)(a));
#define PUSHOUT(a)	out.push_back((Resource*)(a));
#define CLEARINOUT()	in.clear(); out.clear();
#include <cstdio>
#include "NextGenGraphics/Types.hpp"
#include "NextGenGraphics/Context.hpp"
#include "Time/TimeFactory.hpp"
#include "Time/Time.hpp"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	using namespace Eternal::Graphics;
	RenderSettings Settings(1600, 900, DeviceType::VULKAN);
	WindowsArguments WinArguments(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Vulkan", "Vulkan", WindowProc);
	GraphicsContextCreateInformation ContextCreateInformation(Settings, WinArguments);

	FilePath::Register("..\\eternal-engine-shaders\\Shaders\\", FileType::SHADERS);
	FilePath::Register("..\\assets\\textures\\", FileType::TEXTURES);

	ImportTga* TgaImporter = new ImportTga();

	ImmediateTextureFactoryLoadTextureCallback ImmediateLoadTexture;

	GraphicsContext* Context = CreateGraphicsContext(ContextCreateInformation);

	class DebugVulkanCreateGpuResource : public TextureFactoryCreateGpuResourceCallback
	{
	public:
		GraphicsContext& Context;
		DebugVulkanCreateGpuResource(GraphicsContext& InContext)
			: Context(InContext)
		{
		}

		virtual bool CreateTexture(_In_ const RawTextureData& TextureData, _Out_ Texture*& OutTexture) override
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
					uint32_t(StagingBufferMemoryRequirements.size),
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
			vk::Image Texture;
			vk::DeviceMemory TextureMemory;

			vk::ImageCreateInfo TextureInfo(
				vk::ImageCreateFlagBits(),
				vk::ImageType::e2D,
				vk::Format::eR8G8B8A8Unorm,
				vk::Extent3D(TextureData.Width, TextureData.Height, 1),
				1, 0,
				vk::SampleCountFlagBits::e1,
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eStorage,
				vk::SharingMode::eExclusive,
				0, nullptr,
				vk::ImageLayout::eTransferDstOptimal
			);

			Vulkan::VerifySuccess(VulkanDeviceObj.createImage(&TextureInfo, nullptr, &Texture));
			vk::MemoryRequirements TextureMemoryRequirements = VulkanDeviceObj.getImageMemoryRequirements(Texture);

			MemoryFlag = GraphicsMemoryFlag::MEMORY_FLAG_GPU;
			vk::MemoryAllocateInfo TextureMemoryAllocateInfo(
				TextureMemoryRequirements.size,
				static_cast<VulkanDevice&>(Context.GetDevice()).FindBestMemoryTypeIndex(
					uint32_t(StagingBufferMemoryRequirements.size),
					Vulkan::ConvertGraphicsMemoryFlagsToMemoryPropertyFlags(MemoryFlag)
				)
			);

			Vulkan::VerifySuccess(VulkanDeviceObj.allocateMemory(&TextureMemoryAllocateInfo, nullptr, &TextureMemory));
			VulkanDeviceObj.bindImageMemory(Texture, TextureMemory, 0);

			//Context.

			//// Setup buffer copy regions for each mip level
			//std::vector<VkBufferImageCopy> buffer_copy_regions;
			//for (uint32_t i = 0; i < texture.mip_levels; i++)
			//{
			//	ktx_size_t        offset;
			//	KTX_error_code    result                           = ktxTexture_GetImageOffset(ktx_texture, i, 0, 0, &offset);
			//	VkBufferImageCopy buffer_copy_region               = {};
			//	buffer_copy_region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			//	buffer_copy_region.imageSubresource.mipLevel       = i;
			//	buffer_copy_region.imageSubresource.baseArrayLayer = 0;
			//	buffer_copy_region.imageSubresource.layerCount     = 1;
			//	buffer_copy_region.imageExtent.width               = ktx_texture->baseWidth >> i;
			//	buffer_copy_region.imageExtent.height              = ktx_texture->baseHeight >> i;
			//	buffer_copy_region.imageExtent.depth               = 1;
			//	buffer_copy_region.bufferOffset                    = offset;
			//	buffer_copy_regions.push_back(buffer_copy_region);
			//}

			//VkCommandBuffer copy_command = device->create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

			//// Image memory barriers for the texture image

			//// The sub resource range describes the regions of the image that will be transitioned using the memory barriers below
			//VkImageSubresourceRange subresource_range = {};
			//// Image only contains color data
			//subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//// Start at first mip level
			//subresource_range.baseMipLevel = 0;
			//// We will transition on all mip levels
			//subresource_range.levelCount = texture.mip_levels;
			//// The 2D texture only has one layer
			//subresource_range.layerCount = 1;

			//// Transition the texture image layout to transfer target, so we can safely copy our buffer data to it.
			//VkImageMemoryBarrier image_memory_barrier = vkb::initializers::image_memory_barrier();

			//image_memory_barrier.image            = texture.image;
			//image_memory_barrier.subresourceRange = subresource_range;
			//image_memory_barrier.srcAccessMask    = 0;
			//image_memory_barrier.dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT;
			//image_memory_barrier.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
			//image_memory_barrier.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

			//// Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
			//// Source pipeline stage is host write/read exection (VK_PIPELINE_STAGE_HOST_BIT)
			//// Destination pipeline stage is copy command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
			//vkCmdPipelineBarrier(
			//	copy_command,
			//	VK_PIPELINE_STAGE_HOST_BIT,
			//	VK_PIPELINE_STAGE_TRANSFER_BIT,
			//	0,
			//	0, nullptr,
			//	0, nullptr,
			//	1, &image_memory_barrier);

			//// Copy mip levels from staging buffer
			//vkCmdCopyBufferToImage(
			//	copy_command,
			//	staging_buffer,
			//	texture.image,
			//	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			//	static_cast<uint32_t>(buffer_copy_regions.size()),
			//	buffer_copy_regions.data());

			//// Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
			//image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			//image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			//image_memory_barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			//image_memory_barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//// Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
			//// Source pipeline stage stage is copy command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
			//// Destination pipeline stage fragment shader access (VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
			//vkCmdPipelineBarrier(
			//	copy_command,
			//	VK_PIPELINE_STAGE_TRANSFER_BIT,
			//	VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			//	0,
			//	0, nullptr,
			//	0, nullptr,
			//	1, &image_memory_barrier);

			//// Store current layout for later reuse
			//texture.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//device->flush_command_buffer(copy_command, queue, true);

			//// Clean up staging resources
			//vkFreeMemory(get_device().get_handle(), staging_memory, nullptr);
			//vkDestroyBuffer(get_device().get_handle(), staging_buffer, nullptr);

			__debugbreak();

			return false;
		}
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
		{ RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_DYNAMIC_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS, 0 }
	};

	RootSignature* DefaultRootSignature = CreateRootSignature(*Context, &Parameters, 1, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS);
	InputLayout* DefaultInputLayout = CreateInputLayout(*Context);

	vector<BlendState> BlendStates = {
		BlendStateNone
	};

	vector<View*> Views = {
		static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetBackBufferViews()[0]
	};

	RenderPass* RenderPasses[2];
	for (int RenderPassIndex = 0; RenderPassIndex < 2; ++RenderPassIndex)
	{
		vector<View*> Views = {
			static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetBackBufferViews()[RenderPassIndex]
		};

		RenderPassCreateInformation RenderPassInformation(
			Context->GetMainViewport(),
			Views,
			BlendStates
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
	vk::SurfaceKHR& SwapChainSurface = static_cast<VulkanSwapChain&>(Context->GetSwapChain())._Surface;
	vk::SwapchainKHR& SwapChainObj = static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetSwapChain();

	vk::Bool32 CanPresent = false;
	Vulkan::VerifySuccess(PhysDevice.getSurfaceSupportKHR(0, SwapChainSurface, &CanPresent));
	ETERNAL_ASSERT(CanPresent);

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
		1
	);
	vk::CommandBuffer VulkanCommandBuffer;
	Vulkan::VerifySuccess(DeviceObj.allocateCommandBuffers(&CommandBufferInfo, &VulkanCommandBuffer));

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

	GraphicsMemoryFlag MemoryFlag = GraphicsMemoryFlag::MEMORY_FLAG_GPU | GraphicsMemoryFlag::MEMORY_FLAG_MAPPABLE;

	vk::MemoryRequirements BufferMemoryRequirements = DeviceObj.getBufferMemoryRequirements(PerFrameConstantBuffer);
	vk::MemoryAllocateInfo MemoryInfo(
		sizeof(PerFrame),
		static_cast<VulkanDevice&>(Context->GetDevice()).FindBestMemoryTypeIndex(
			uint32_t(BufferMemoryRequirements.size * 2),
			Vulkan::ConvertGraphicsMemoryFlagsToMemoryPropertyFlags(MemoryFlag)
		)
	);

	const uint32_t BufferSize = BufferMemoryRequirements.size;

	vk::DeviceMemory BufferMemory;
	Vulkan::VerifySuccess(DeviceObj.allocateMemory(&MemoryInfo, nullptr, &BufferMemory));

	vk::DescriptorPoolSize PoolSize(
		vk::DescriptorType::eUniformBufferDynamic,
		16
	);

	vk::DescriptorPoolCreateInfo DescriptorPoolInfo(
		vk::DescriptorPoolCreateFlagBits(),
		2,
		1, &PoolSize
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

	TextureFactoryRequest Request("Noise", ".\\noise.tga");
	TexFactory.CreateRequest(Request);

	Eternal::Time::Time* Timer = Eternal::Time::CreateTime(Eternal::Time::TimeType::WIN);

	double ElapsedTime = 0.0;

	static int i = 0;
	for (;;)
	{
		Timer->Update();
		ElapsedTime += Timer->GetDeltaTimeSeconds();

		TexFactory.ProcessRequests();

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
		
		vk::CommandBufferBeginInfo CommandBufferBegin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		Vulkan::VerifySuccess(VulkanCommandBuffer.begin(&CommandBufferBegin));

		const vk::Image& BackBufferImage = static_cast<VulkanSwapChain&>(Context->GetSwapChain()).GetBackBufferImages()[ImageIndex];

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

		vk::DeviceSize Range = sizeof(PerFrame);

		vk::DescriptorBufferInfo BufferDescriptorInfo(
			PerFrameConstantBuffer,
			0, Range
		);

		vk::WriteDescriptorSet NewDescriptorSet(
			FrameConstantsDescriptorSet,
			0, 0, 1, vk::DescriptorType::eUniformBufferDynamic,
			nullptr, &BufferDescriptorInfo, nullptr
		);

		DeviceObj.updateDescriptorSets(
			1, &NewDescriptorSet,
			0, nullptr
		);

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
			1, &FrameConstantsDescriptorSet,
			1, &DynamicOffset
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

		vk::PipelineStageFlags WaitDestStageMask = vk::PipelineStageFlagBits::eAllGraphics;
		vk::SubmitInfo SubmitInfo(
			1, &CurrentSemaphore,
			&WaitDestStageMask,
			1, &VulkanCommandBuffer,
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

	DestroyGraphicsContext(Context);
	

//
//	//new D3D12ShaderFactory();
//	//D3D12Device::Initialize();
//	//D3D12Device DeviceObj(0);
//
//	VulkanDevice DeviceObj(WindowObj);
//	CommandQueue& DirectCommandQueue = *CreateCommandQueue(DeviceObj, 2);
//	SwapChain& SwapChainObj = *CreateSwapChain(DeviceObj, WindowObj, DirectCommandQueue);
//
//	//Shader* VS = D3D12ShaderFactory::Get()->CreateVertexShader("PostProcess", "postprocess.vs.hlsl");
//	//Shader* PS = D3D12ShaderFactory::Get()->CreatePixelShader("DefaultPostProcess", "defaultpostprocess.ps.hlsl");
//	
//	VulkanShader VS(DeviceObj, "PostProcessVS", "postprocess.vs.hlsl", VS);
//	VulkanShader PS(DeviceObj, "DefaultPostProcessPS", "defaultpostprocess.ps.hlsl", PS);
//	
//	VulkanHeap HeapTexture(DeviceObj, 1024 * 4, 2, true, false, false, false);
//	VulkanHeap HeapStaging(DeviceObj, 1024 * 4, 2, false, true, true, false);
//
//	VulkanResource BufferObj(DeviceObj, HeapTexture, 1024 * 4, (ResourceType)(BUFFER_STRUCTURED_READ | BUFFER_TRANSFER_WRITE));
//	VulkanResource StagingObj(DeviceObj, HeapStaging, 1024 * 4, BUFFER_TRANSFER_READ);
//
//	//TextureObj.CreateView(DeviceObj, )
//	RootSignature& RootSignatureObj = *CreateRootSignature(DeviceObj);
//	VulkanDescriptorHeap DescriptorHeapObj(DeviceObj, STRUCTURED_BUFFER, 2, (RootSignatureAccess)(ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
//	DescriptorHeap* DescriptorHeaps[]
//	{
//		&DescriptorHeapObj
//	};
//	RootSignature& RootSignatureWithSB = *CreateRootSignature(DeviceObj, nullptr, 0u, DescriptorHeaps, 1, (RootSignatureAccess)(ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
//
//	View* ViewObj = BufferObj.CreateView(DeviceObj, DescriptorHeapObj, BGRA8888);
//
//	//InputLayout::VertexDataType Vertices[] = {
//	//	InputLayout::POSITION_T
//	//};
//	//D3D12InputLayout InputLayoutObj(Vertices, ETERNAL_ARRAYSIZE(Vertices));
//	//DepthTest DepthTestObj;
//	//StencilTest StencilTestObj;
//	//BlendState BlendStates[] = {
//	//	BlendState(BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD, BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState()
//	//};
//	//D3D12InputLayout InputLayoutObj(Vertices, ETERNAL_ARRAYSIZE(Vertices));
//	//DepthTest DepthTestObj;
//	//StencilTest StencilTestObj;
//	//BlendState BlendStates[] = {
//	//	BlendState(BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD, BlendState::ONE, BlendState::ZERO, BlendState::OP_ADD),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState(),
//	//	BlendState()
//	//};
//
//	//D3D12State StateObj(DeviceObj, InputLayoutObj, VS, PS, DepthTestObj, StencilTestObj, BlendStates, /*SwapChainObj.GetBackBuffer(0),*/ 1, nullptr, 0);
//
//	//D3D12CommandList CommandLists(DeviceObj, DirectCommandQueue, StateObj);
//	VulkanCommandList CommandLists[] = {
//		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(0)),
//		VulkanCommandList(DeviceObj, *DirectCommandQueue.GetCommandAllocator(1))
//	};
//
//	//D3D12Pipeline PipelineObj(DeviceObj, RootSignatureObj, InputLayoutObj, VS, PS, DepthTestObj, StencilTestObj, BlendStates, /*SwapChainObj.GetBackBuffer(0),*/ 1, nullptr, 0);
//
//	Fence& FenceObj = *CreateFence(DeviceObj, 2);
//	VulkanPipeline PipelineObj(DeviceObj, RootSignatureWithSB, SwapChainObj.GetMainRenderPass(), VS, PS, ViewportObj);
//
//	VkDescriptorSet DescriptorSets[] =
//	{
//		static_cast<VulkanDescriptorHeap&>(DescriptorHeapObj).Pop(),
//		static_cast<VulkanDescriptorHeap&>(DescriptorHeapObj).Pop()
//	};
//
	for (;;)
	{
//		FenceObj.Wait(DeviceObj);
//		FenceObj.Reset(DeviceObj);
//		uint32_t CurrentFrame = SwapChainObj.AcquireFrame(DeviceObj, FenceObj);
//		DirectCommandQueue.Reset(CurrentFrame);
//		//CommandLists.Begin(*DirectCommandQueue.GetCommandAllocator(CurrentFrame), PipelineObj);
//		//CommandLists.SetViewport(ViewportObj);
//		//CommandLists.SetScissorRectangle(ViewportObj);
//		//CommandLists.GetD3D12GraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		//static_cast<D3D12RenderTarget&>(SwapChainObj.GetBackBuffer(CurrentFrame)).Transition(CommandLists, RENDERTARGET);
//		//CommandLists.BindRenderTarget(0, SwapChainObj.GetBackBufferView(CurrentFrame));
//		//CommandLists.DrawPrimitive(6);
//		//static_cast<D3D12RenderTarget&>(SwapChainObj.GetBackBuffer(CurrentFrame)).Transition(CommandLists, PRESENT);
//		//CommandLists.End();
//
//		CommandLists[CurrentFrame].Begin(*DirectCommandQueue.GetCommandAllocator(CurrentFrame), PipelineObj);
//		
//		void* Data = StagingObj.Map(DeviceObj);
//		for (int i = 0; i < 1024; ++i)
//		{
//			((int*)Data)[i] = 0xFF | (i % 0xFF) << 8 | ((i >> 8) * 0xFF) << 24;
//		}
//		StagingObj.Unmap(DeviceObj);
//
//		ResourceTransition CopyBufferTransitions[] = {
//			ResourceTransition(&StagingObj, TRANSITION_UNDEFINED, TRANSITION_TRANSFER_READ),
//			ResourceTransition(&BufferObj, TRANSITION_UNDEFINED, TRANSITION_TRANSFER_WRITE)
//		};
//		CommandLists[CurrentFrame].Transition(CopyBufferTransitions, ETERNAL_ARRAYSIZE(CopyBufferTransitions), nullptr, 0);
//		CommandLists[CurrentFrame].CopyBuffer(StagingObj, BufferObj);
//
//		ResourceTransition UseBufferTransition(&BufferObj, TRANSITION_TRANSFER_WRITE, TRANSITION_SHADER_READ);
//		CommandLists[CurrentFrame].Transition(&UseBufferTransition, 1, nullptr, 0);
//
//		CommandLists[CurrentFrame].BeginRenderPass(SwapChainObj.GetMainRenderPass(), SwapChainObj.GetBackBufferView(CurrentFrame).GetAsRenderTarget(), ViewportObj);
//
//#pragma region DescriptorSetStuffTest
//		VkDescriptorBufferInfo BufferDescription;
//		BufferDescription.buffer = static_cast<VulkanResource&>(BufferObj).GetBuffer();
//		BufferDescription.offset = 0;
//		BufferDescription.range = 1024 * 4;
//
//		VkWriteDescriptorSet WriteDescriptorSet;
//		WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		WriteDescriptorSet.pNext = nullptr;
//		WriteDescriptorSet.dstSet = DescriptorSets[CurrentFrame];
//		WriteDescriptorSet.dstBinding = 0;
//		WriteDescriptorSet.dstArrayElement = 0;
//		WriteDescriptorSet.descriptorCount = 1;
//		WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
//		WriteDescriptorSet.pImageInfo = nullptr;
//		WriteDescriptorSet.pBufferInfo = &BufferDescription;
//		WriteDescriptorSet.pTexelBufferView = &static_cast<VulkanView*>(ViewObj)->GetBufferView();
//
//		vkUpdateDescriptorSets(static_cast<VulkanDevice&>(DeviceObj).GetVulkanDevice(), 1, &WriteDescriptorSet, 0, nullptr);
//
//		vkCmdBindDescriptorSets(CommandLists[CurrentFrame].GetVulkanCommandList(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VulkanRootSignature&>(RootSignatureWithSB).GetPipelineLayout(), 0, 1, &DescriptorSets[CurrentFrame], 0, nullptr);
//#pragma endregion DescriptorSetStuffTest
//
//		CommandLists[CurrentFrame].DrawPrimitive(6);
//		CommandLists[CurrentFrame].EndRenderPass();
//		CommandLists[CurrentFrame].End();
//
//		FenceObj.Reset(DeviceObj);
//		CommandList* VulkanCommandLists[] = {
//			&CommandLists[CurrentFrame]
//			//&CommandLists
//		};
//		DirectCommandQueue.Submit(CurrentFrame, VulkanCommandLists, ETERNAL_ARRAYSIZE(VulkanCommandLists), FenceObj, SwapChainObj);
//		FenceObj.Signal(DirectCommandQueue);
//		SwapChainObj.Present(DeviceObj, DirectCommandQueue, CurrentFrame);

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

	//delete ReShieldPreGameStateObj;
	//ReShieldPreGameStateObj = nullptr;

	return 0;
}
