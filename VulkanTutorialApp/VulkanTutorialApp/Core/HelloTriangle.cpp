#include "HelloTriangle.h"

#include <glm/glm.hpp>

namespace Core
{
	HelloTriangle::HelloTriangle() : 
		  _pWindow(nullptr)
		, _pAllocator(VK_NULL_HANDLE)
		, _instance(VK_NULL_HANDLE)
		, _debugCallback(VK_NULL_HANDLE)
		, _physicalDevice(VK_NULL_HANDLE)
		, _device(VK_NULL_HANDLE)
		, _graphicsQueue(VK_NULL_HANDLE)
		, _presentQueue(VK_NULL_HANDLE)
		, _surface(VK_NULL_HANDLE)
		, _swapChain(VK_NULL_HANDLE)
	{
	}

	HelloTriangle::~HelloTriangle()
	{
	}

	void HelloTriangle::Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangle::DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg, void * userData)
	{
		HelloTriangle* pApp = static_cast<HelloTriangle*>(userData);

		JE_PrintLineErr("Validation layer: " + std::string(msg));

		return VK_FALSE;
	}

	void HelloTriangle::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
	}

	void HelloTriangle::InitVulkan()
	{
		CreateInstance();
		SetupDebugCallback();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDeviceAndGetQueues();
		CreateSwapChain();
		CreateSwapChainImageViews();
	}

	void HelloTriangle::CreateInstance()
	{
		if (_bEnableValidationLayers && !CheckValidationLayerSupport())
		{
			JE_AssertThrow(false, "Validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = WINDOW_NAME;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "JadeEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions;
		GetRequiredExtensions(extensions);
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (_bEnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		JE_AssertThrowVkResult(vkCreateInstance(&createInfo, _pAllocator, &_instance));

		
		// Look for optional extensions.

		uint32_t availableExtensionsCount;
		JE_AssertThrowVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr));
		std::vector<VkExtensionProperties> extensionProperties(availableExtensionsCount);
		JE_AssertThrowVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, extensionProperties.data()));

		JE_PrintLine("Vulkan available extensions:");
		for (const auto& extensionProp : extensionProperties)
		{
			JE_PrintLine("\t" + std::string(extensionProp.extensionName));
		}
	}

	bool HelloTriangle::CheckValidationLayerSupport()
	{
		uint32_t layerCount = 0;
		JE_AssertThrowVkResult(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
		if (layerCount == 0)
			return false;
		std::vector<VkLayerProperties> availableLayers(layerCount);
		JE_AssertThrowVkResult(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

		for (const char* layerName : _validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	void HelloTriangle::GetRequiredExtensions(std::vector<const char*>& outExtensions)
	{
		outExtensions.clear();

		uint32_t extensionCount = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
		if (extensionCount == 0)
		{
			return;
		}
		
		for (size_t i = 0; i < extensionCount; ++i)
		{
			outExtensions.push_back(extensions[i]);
		}

		// Place for additional extensions that are required.
		if (_bEnableValidationLayers)
		{
			outExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
	}

	void HelloTriangle::SetupDebugCallback()
	{
		if (!_bEnableValidationLayers)
			return;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;
		createInfo.pUserData = this;

		JE_AssertThrowVkResult(CallVkProc(vkCreateDebugReportCallbackEXT, _instance, &createInfo, _pAllocator, &_debugCallback));
	}

	void HelloTriangle::CreateSurface()
	{
		JE_AssertThrowVkResult(glfwCreateWindowSurface(_instance, _pWindow, _pAllocator, &_surface));
	}

	void HelloTriangle::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
		JE_AssertThrow(deviceCount != 0, "No physical device found!");
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());

		std::multimap<uint32_t, VkPhysicalDevice> deviceRating;

		for (const auto& device : physicalDevices)
		{
			deviceRating.emplace(RateDeviceSuitability(device), device);
		}

		JE_AssertThrow
		(
			deviceRating.size() > 0 && 
			deviceRating.rbegin()->first > 0 && 
			deviceRating.rbegin()->second != VK_NULL_HANDLE
			, "No suitable physical device found!"
		);

		_physicalDevice = deviceRating.rbegin()->second;
	}

	uint32_t HelloTriangle::RateDeviceSuitability(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);

		uint32_t rating = 0;

		rating += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 2 : 0;
		rating += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? 1 : 0;
		rating += properties.apiVersion;
		rating += features.shaderFloat64 ? 1 : 0;
		rating += features.geometryShader ? 1 : 0;
		rating += features.tessellationShader ? 1 : 0;

		QueueFamilyIndices queueFamilyIndices;
		FindQueueFamilies(physicalDevice, queueFamilyIndices);
		if (!queueFamilyIndices.IsComplete())
			return 0;

		if (!CheckDeviceExtensionSupport(physicalDevice))
			return 0;

		SwapChainSupportDetails swapChainSupportDetails;
		QuerySwapChainSupport(physicalDevice, swapChainSupportDetails);
		if (
				swapChainSupportDetails.Formats.empty()
			||	swapChainSupportDetails.PresentModes.empty()
			)
		{
			return 0;
		}

		return rating;
	}

	void HelloTriangle::FindQueueFamilies(VkPhysicalDevice physicalDevice, QueueFamilyIndices& outIndices)
	{
		JE_AssertThrowDefault(physicalDevice != VK_NULL_HANDLE);

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		JE_AssertThrow(queueFamilyCount > 0, "No Queue Families found!");

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int32_t i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0)
			{
				if (queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
				{
					outIndices.GraphicsFamily = i;
				}

				VkBool32 presentSupport = false;
				JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport));
				if (presentSupport)
				{
					outIndices.PresentFamily = i;
				}
			}

			if (outIndices.IsComplete())
			{
				break;
			}

			++i;
		}
	}

	bool HelloTriangle::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void HelloTriangle::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, SwapChainSupportDetails & outSupportDetails)
	{
		outSupportDetails.Formats.clear();
		outSupportDetails.PresentModes.clear();

		// Basic surface capabilities.

		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface, &outSupportDetails.Capabilities));

		// Supported surface formats.

		uint32_t formatCount;
		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, nullptr));
		outSupportDetails.Formats.resize(formatCount);
		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, outSupportDetails.Formats.data()));

		// Supported present modes.

		uint32_t presentModeCount;
		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, nullptr));
		outSupportDetails.PresentModes.resize(presentModeCount);
		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, outSupportDetails.PresentModes.data()));
	}

	void HelloTriangle::CreateLogicalDeviceAndGetQueues()
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(_physicalDevice, indices);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int32_t> uniqueQueueFamilies = { indices.GraphicsFamily, indices.PresentFamily };

		float queuePriority = 1.0f;
		for (int32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		// TODO: Enable more features.

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

		if (_bEnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		JE_AssertThrowVkResult(vkCreateDevice(_physicalDevice, &createInfo, _pAllocator, &_device));

		vkGetDeviceQueue(_device, indices.GraphicsFamily, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, indices.PresentFamily, 0, &_presentQueue);
	}

	void HelloTriangle::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport;
		QuerySwapChainSupport(_physicalDevice, swapChainSupport);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;	// This is always 1 unless you are developing a stereoscopic 3D application.
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	// It is also possible that you'll render images to a separate image first to perform operations like post-processing. In that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image.

		// Specify how to handle swap chain images.

		QueueFamilyIndices indices;
		FindQueueFamilies(_physicalDevice, indices);
		uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.GraphicsFamily), static_cast<uint32_t>(indices.PresentFamily) };

		if (indices.GraphicsFamily != indices.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;


		JE_AssertThrowVkResult(vkCreateSwapchainKHR(_device, &createInfo, _pAllocator, &_swapChain));


		// Store necessary data.
		_swapChainFormat = createInfo.imageFormat;
		_swapChainExtent = createInfo.imageExtent;
	}

	VkSurfaceFormatKHR HelloTriangle::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		JE_AssertThrow(availableFormats.size() != 0, "No format is available for swap surface!");

		const VkFormat DesiredFormat = VK_FORMAT_B8G8R8A8_UNORM;
		const VkColorSpaceKHR DesiredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		// In case when Vulkan returns only one available format VK_FORMAT_UNDEFINED, it means it supports any format we want.
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { DesiredFormat, DesiredColorSpace };
		}

		// In any other case, let's look through the list of available formats.
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == DesiredFormat && availableFormat.colorSpace == DesiredColorSpace)
			{
				return availableFormat;
			}
		}

		// Dumb way to pick format if everything else fails.
		return availableFormats[0];
	}

	VkPresentModeKHR HelloTriangle::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)	// We prefer this becuse some drivers don't support FIFO (though it's always available in Vulkan. Weird.)
				bestMode = availablePresentMode;
		}

		// This mode is always guaranteed to be available.
		return bestMode;
	}

	VkExtent2D HelloTriangle::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
	{
		VkExtent2D finalExtent = { static_cast<uint32_t>(WINDOW_WIDTH), static_cast<uint32_t>(WINDOW_HEIGHT) };

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			finalExtent = capabilities.currentExtent;
		}
		else
		{
			finalExtent.width = glm::clamp<uint32_t>(finalExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.height);
			finalExtent.height = glm::clamp<uint32_t>(finalExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		}

		JE_Print("Choosing swap chain surface extent: ");
		JE_Print(finalExtent.width);
		JE_Print(" x ");
		JE_PrintLine(finalExtent.height);

		return finalExtent;
	}

	void HelloTriangle::RetrieveSwapChainImages()
	{
		// The implementation is allowed to create more images, which is why we need to explicitly query the amount again.

		uint32_t imageCount;
		JE_AssertThrowVkResult(vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr));
		_swapChainImages.resize(imageCount);
		JE_AssertThrowVkResult(vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data()));
	}

	void HelloTriangle::CreateSwapChainImageViews()
	{
		_swapChainImageViews.resize(_swapChainImages.size());

		for (size_t i = 0; i < _swapChainImages.size(); ++i)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _swapChainImages[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _swapChainFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			JE_AssertThrowVkResult(vkCreateImageView(_device, &createInfo, _pAllocator, &_swapChainImageViews[i]));
		}
	}

	void HelloTriangle::MainLoop()
	{
		while (!glfwWindowShouldClose(_pWindow))
		{
			glfwPollEvents();
		}
	}

	void HelloTriangle::Cleanup()
	{
		for (auto imageView : _swapChainImageViews)
		{
			vkDestroyImageView(_device, imageView, _pAllocator);
		}

		vkDestroySwapchainKHR(_device, _swapChain, _pAllocator);

		vkDestroyDevice(_device, _pAllocator);

		CleanupDebugCallback();

		vkDestroySurfaceKHR(_instance, _surface, _pAllocator);

		vkDestroyInstance(_instance, _pAllocator);

		glfwDestroyWindow(_pWindow);
		glfwTerminate();
	}

	void HelloTriangle::CleanupDebugCallback()
	{
		if (!_bEnableValidationLayers)
			return;

		CallVkProc(vkDestroyDebugReportCallbackEXT, _instance, _debugCallback, _pAllocator);
	}
}