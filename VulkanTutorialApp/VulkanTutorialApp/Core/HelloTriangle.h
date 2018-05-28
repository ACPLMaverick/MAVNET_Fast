#pragma once

#include "GlobalIncludes.h"

namespace Core
{
	class HelloTriangle
	{
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

		const VkClearValue _clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

		enum ShaderType
		{
			Vertex,
			TesselationControl,
			TesselationEvaluation,
			Geometry,
			Fragment,
			Compute,
			NUM
		};

		static const char* ShaderTypeToExtension[ShaderType::NUM];

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

		struct alignas(16) VertexTutorial
		{
			static const size_t COMPONENT_NUMBER = 4;

			glm::vec3 Position;
			glm::vec3 Color;
			glm::vec3 Normal;
			glm::vec2 Uv;

			static void GetBindingDescription(VkVertexInputBindingDescription& outDescription);
			static void GetAttributeDescription(std::vector<VkVertexInputAttributeDescription>& outDescriptions);
		};

		struct UniformBufferObject
		{
			glm::mat4 MVP;
			glm::mat4 MV;
			glm::mat4 MVInverseTranspose;

			UniformBufferObject()
				: MVP(glm::mat4(1.0f))
				, MV(glm::mat4(1.0f))
				, MVInverseTranspose(glm::mat4(1.0f))
			{
			}
		};

		const std::vector<VertexTutorial> _vertices = 
		{
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
			{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
			{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
			{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } }
		};

		const std::vector<uint16_t> _indices =
		{
			0, 1, 2, 2, 3, 0
		};

	public:
		HelloTriangle();
		~HelloTriangle();

		void Run();

	private:

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
			void CreateRenderPass();
			void CreateDescriptorSetLayout();
			void CreateGraphicsPipeline();
				VkShaderModule CreateShaderModule(const std::vector<uint8_t> code);
			void CreateFramebuffers();
			void CreateCommandPool();
			void CreateDescriptorPool();
			void CreateVertexBuffer();
			void CreateIndexBuffer();
			void CreateUniformBuffer();
			void CreateCommandBuffers();
			void CreateDescriptorSet();
			void CreateSyncObjects();

		void MainLoop();
			void DrawFrame();
			void CheckForMinimized();
			void UpdateUniformBuffer();

		void RecreateSwapChain();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory);
		void CopyBuffer_CPU_GPU(const void* srcData, VkDeviceMemory dstMemory, size_t copySize);
		void CopyBuffer_GPU_GPU(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize copySize);

		void Cleanup();
			void CleanupDebugCallback();
			void CleanupSwapChain();


		static void LoadFile(const std::string& fileName, std::vector<uint8_t>& outData);
		static void LoadShader(const std::string& shaderName, ShaderType shaderType, std::vector<uint8_t>& outData);

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

		VkRenderPass _renderPass;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _graphicsPipeline;

		std::vector<VkFramebuffer> _swapChainFramebuffers;

		VkCommandPool _commandPool;
		VkCommandPool _commandPoolTransient;
		std::vector<VkCommandBuffer> _commandBuffers;

		VkDescriptorPool _descriptorPool;

		std::vector<VkSemaphore> _semsImageAvailable;
		std::vector<VkSemaphore> _semsRenderFinished;
		std::vector<VkFence> _fencesInFlight;

		uint32_t _currentFrame;
		UniformBufferObject _ubo;

		VkDescriptorSet _descriptorSet;

		VkDeviceMemory _vertexBufferMemory;
		VkDeviceMemory _indexBufferMemory;
		VkDeviceMemory _uniformBufferMemory;

		VkBuffer _vertexBuffer;
		VkBuffer _indexBuffer;
		VkBuffer _uniformBuffer;

		bool _bMinimized;
	};
}