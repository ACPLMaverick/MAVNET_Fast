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

#if NDEBUG
		const bool _bEnableValidationLayers = false;
#else
		const bool _bEnableValidationLayers = true;
#endif

	public:
		HelloTriangle();
		~HelloTriangle();

		void Run();

	private:

		struct QueueFamilyIndices
		{
			int32_t GraphicsFamily = -1;

			bool IsComplete()
			{
				return GraphicsFamily >= 0;
			}
		};


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


		void InitWindow();

		void InitVulkan();
			void CreateInstance();
				bool CheckValidationLayerSupport();
				void GetRequiredExtensions(std::vector<const char*>& outExtensions);
			void SetupDebugCallback();
			void PickPhysicalDevice();
				uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
				void FindQueueFamilies(VkPhysicalDevice physicalDevice, QueueFamilyIndices& outIndices);
			void CreateLogicalDeviceAndGetQueues();

		void MainLoop();

		void Cleanup();
			void CleanupDebugCallback();


		PFN_vkVoidFunction GetVkProcVoid(const char* procName)
		{
			JE_AssertThrowDefault(procName != nullptr && _instance != nullptr);
			PFN_vkVoidFunction ret = vkGetInstanceProcAddr(_instance, procName);
			JE_AssertThrowDefault(ret != nullptr);
			return ret;
		}

		#define GetVkProc(procType) GetVkProcInternal<PFN_##procType>(#procType)
		template <typename FuncT> FuncT GetVkProcInternal(const char* procName)
		{
			return reinterpret_cast<FuncT>(GetVkProcVoid(procName));
		}


		GLFWwindow* _pWindow;

		VkAllocationCallbacks* _pAllocator;
		VkInstance _instance;
		VkDebugReportCallbackEXT _debugCallback;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		VkQueue _graphicsQueue;
	};
}