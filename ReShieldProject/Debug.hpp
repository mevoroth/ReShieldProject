#pragma once

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
#include "Time/Timer.hpp"
#include "Log/LogFactory.hpp"
#include "Log/Log.hpp"
#include "Parallel/ParallelSystem.hpp"
#include "Optick/Optick.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include "optick.h"

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
	}
}

using namespace Eternal::Graphics;

void SampleRenderGeneric(GraphicsContext* Context, Eternal::InputSystem::Input* MultiInputHandle);
