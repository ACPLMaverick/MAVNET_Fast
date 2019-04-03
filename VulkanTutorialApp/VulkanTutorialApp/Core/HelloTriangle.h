#pragma once

#include "Core/ManagerUid.h"
#include "Core/resource/ResourceManager.h"
#include "Core/Timer.h"

#include "Rendering/Camera.h"
#include "Rendering/LightDirectional.h"
#include "Rendering/Fog.h"

#include "Rendering/resource/Texture.h"
#include "Rendering/resource/Mesh.h"
#include "Rendering/resource/Material.h"
#include "Rendering/sampler/ManagerSampler.h"
#include "Rendering/descriptor/ManagerDescriptor.h"
#include "Rendering/pipeline/ManagerPipeline.h"
#include "Rendering/renderPass/ManagerRenderPass.h"

#include "GOM/World.h"
#include "GOM/system/System.h"

#include "Util/Random.h"

namespace Core
{
	class HelloTriangle
	{
	public:
		
		static const std::string RESOURCE_PATH;

	public:

		const int32_t WINDOW_WIDTH = 800;
		const int32_t WINDOW_HEIGHT = 600;
		const char* WINDOW_NAME = "Vulkan - HelloTriangle";

	private:

		const std::vector<const char*> _validationLayers =
		{
			"VK_LAYER_LUNARG_standard_validation"
		};

		const std::vector<const char*> _deviceExtensions = 
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

#if NDEBUG
		const bool _bEnableValidationLayers = false;
#else
		const bool _bEnableValidationLayers = true;
#endif

		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

		const std::string MODEL_NAME_MESH = "chalet.obj";

		struct QueueFamilyIndices
		{
			int32_t GraphicsFamily = -1;
			int32_t PresentFamily = -1;	// Families used for rendering and presentation may differ.

			bool IsComplete()
			{
				return GraphicsFamily >= 0 && PresentFamily >= 0;
			}
		};

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

	public:
		HelloTriangle();
		~HelloTriangle();

		void Run();

		static HelloTriangle* GetInstance() { JE_Assert(_singletonInstance != nullptr); return _singletonInstance; }

		static bool LoadFile(const std::string& fileName, std::vector<uint8_t>& outData);

		VkDevice GetDevice() { return _device; }
		VkAllocationCallbacks* GetAllocatorPtr() { return _pAllocator; }
		VkCommandPool GetCommandPoolDynamic() { return _commandPoolDynamic; }
		VkCommandPool GetCommandPoolStatic() { return _commandPoolStatic; }
		VkCommandBuffer GetCmd() { return _commandBuffers[_imageIndex]; }
		::Rendering::RenderPassCommon::Id GetActiveRenderPass(uint32_t* outSubpass);

		::Rendering::Camera* GetCamera() { return &_camera; }

		ManagerUid* GetManagerUid() { return &_uidMgr; }
		::Rendering::ManagerSampler* GetManagerSampler() { return &_samplerMgr; }
		::Rendering::ManagerDescriptor* GetManagerDescriptor() { return &_descriptorMgr; }
		::Rendering::ManagerPipeline* GetManagerPipeline() { return &_pipelineMgr; }
		::Rendering::ManagerRenderPass* GetManagerRenderPass() { return &_renderPassMgr; }

		ResourceManager* GetResourceManager() { return &_resourceManager; }

		::GOM::System* GetSystem() { return &_system; }
		::GOM::World* GetWorld() { return &_world; }

		::Util::Random* GetRandom() { return &_random; }

		Timer* GetGlobalTimer() { return &_timer; }


		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void TransitionImageLayout(const ::Rendering::Texture::Info* texInfo, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory);
		void CopyBuffer_CPU_GPU(const void* srcData, VkDeviceMemory dstMemory, size_t copySize);
		void CopyBuffer_GPU_GPU(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize copySize);
		void CreateImage(const ::Rendering::Texture::Info* texInfo, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties, VkImage& outImage, VkDeviceMemory& outMemory);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, const ::Rendering::Texture::Info* texInfo);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidateFormats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);

		VkFormat GetSwapChainFormat() { return _swapChainFormat; }

		VkExtent2D GetSwapChainExtent() { return _swapChainExtent; }

	private:

		static HelloTriangle* _singletonInstance;

		void InitWindow();

		void InitVulkan();
			void CreateInstance();
				bool CheckValidationLayerSupport();
				void GetRequiredExtensions(std::vector<const char*>& outExtensions);
			void SetupDebugCallback();
			void CreateSurface();
			void PickPhysicalDevice();
				uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
				void FindQueueFamilies(VkPhysicalDevice physicalDevice, QueueFamilyIndices& outIndices);
				bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
				void QuerySwapChainSupport(VkPhysicalDevice physicalDevice, SwapChainSupportDetails& outSupportDetails);
			void CreateLogicalDeviceAndGetQueues();
			void CreateSwapChain();
				VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
				VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
				VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
			void RetrieveSwapChainImages();
			void CreateSwapChainImageViews();

			void InitObjects();
			void RefreshCameraProj(uint32_t newWidth, uint32_t newHeight);

			void CreateFramebuffers();
			void CreateCommandPool();
			void CreateDepthResources();
			void CreateCommandBuffers();
			// void CreatePushConstantRange();
			void CreateSyncObjects();

		void MainLoop();
			void UpdateObjects();
			void DrawFrame();
			void CreateCommandBuffer();
			void CheckForMinimized();

		void RecreateSwapChain();

		void Cleanup();
			void CleanupObjects();
			void CleanupDebugCallback();
			void CleanupSwapChain();

		PFN_vkVoidFunction GetVkProcVoid(const char* procName)
		{
			JE_AssertThrow(procName != nullptr, "Null argument.");
			JE_AssertThrow(_instance != nullptr, "Instance not initialized.");
			PFN_vkVoidFunction ret = vkGetInstanceProcAddr(_instance, procName);
			JE_AssertThrow(ret != nullptr, "Bad input function name, returned nullptr.");
			return ret;
		}

		template <typename FuncT> FuncT GetVkProcInternal(const char* procName)
		{
			return reinterpret_cast<FuncT>(GetVkProcVoid(procName));
		}

		#define GetVkProc(procType) GetVkProcInternal<PFN_##procType>(#procType)
		#define CallVkProc(procType, ...) (*(GetVkProc(procType)))(__VA_ARGS__)

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
		(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData
		);


		GLFWwindow* _pWindow;

		VkAllocationCallbacks* _pAllocator;
		VkPipelineCache* _pPipelineCache;
		VkInstance _instance;
		VkDebugReportCallbackEXT _debugCallback;
		VkPhysicalDevice _physicalDevice;
		
		VkDevice _device;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;

		VkSurfaceKHR _surface;

		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		VkFormat _swapChainFormat;
		VkExtent2D _swapChainExtent;

		std::vector<VkFramebuffer> _swapChainFramebuffers;

		VkCommandPool _commandPoolDynamic;
		VkCommandPool _commandPoolStatic;
		VkCommandPool _commandPoolTransient;
		std::vector<VkCommandBuffer> _commandBuffers;

		VkPushConstantRange _pushConstantRange; // TODO: Delegate elsewhere.

		std::vector<VkSemaphore> _semsImageAvailable;
		std::vector<VkSemaphore> _semsRenderFinished;
		std::vector<VkFence> _fencesInFlight;

		uint32_t _currentFrame;
		uint32_t _imageIndex;

		::Rendering::RenderPassKey _currentRenderPassKey;


		// ++TODO delegate elsewhere.
		::Rendering::Camera _camera;
		// --

		VkDeviceMemory _depthImageMemory;

		VkImage _depthImage;
		VkImageView _depthImageView;

		ManagerUid _uidMgr;

		::Rendering::ManagerSampler _samplerMgr;
		::Rendering::ManagerDescriptor _descriptorMgr;
		::Rendering::ManagerPipeline _pipelineMgr;
		::Rendering::ManagerRenderPass _renderPassMgr;

		ResourceManager _resourceManager;
		::GOM::System _system;

		// Delegate this to GOM or real core.
		::GOM::World _world;
		::Util::Random _random;
		::Core::Timer _timer;

		bool _bMinimized;
	};
}

#define JE_GetRenderer() Core::HelloTriangle::GetInstance()