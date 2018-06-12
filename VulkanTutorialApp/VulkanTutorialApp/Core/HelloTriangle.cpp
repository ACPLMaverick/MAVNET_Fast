#include "HelloTriangle.h"

#include <fstream>

#define USE_STAGING_BUFFER 1

static const std::string RESOURCE_PATH = "..\\..\\JadeEngine\\JadeEngine\\Resources\\";

namespace Core
{
	const char* HelloTriangle::ShaderTypeToExtension[]
	{
		".vert",
		".tesc",
		".tese",
		".geom",
		".frag",
		".comp"
	};

	HelloTriangle* HelloTriangle::_singletonInstance = nullptr;

	HelloTriangle::HelloTriangle() : 
		  _pWindow(nullptr)
		, _pAllocator(VK_NULL_HANDLE)
		, _pPipelineCache(VK_NULL_HANDLE)
		, _instance(VK_NULL_HANDLE)
		, _debugCallback(VK_NULL_HANDLE)
		, _physicalDevice(VK_NULL_HANDLE)
		, _device(VK_NULL_HANDLE)
		, _graphicsQueue(VK_NULL_HANDLE)
		, _presentQueue(VK_NULL_HANDLE)
		, _surface(VK_NULL_HANDLE)
		, _swapChain(VK_NULL_HANDLE)
		, _renderPass(VK_NULL_HANDLE)
		, _pipelineLayout(VK_NULL_HANDLE)
		, _graphicsPipeline(VK_NULL_HANDLE)
		, _commandPool(VK_NULL_HANDLE)
		, _descriptorPool(VK_NULL_HANDLE)
		, _currentFrame(0)
		, _descriptorSet(VK_NULL_HANDLE)
		, _vertexBufferMemory(VK_NULL_HANDLE)
		, _indexBufferMemory(VK_NULL_HANDLE)
		, _uniformBufferMemory(VK_NULL_HANDLE)
		, _textureImageMemory(VK_NULL_HANDLE)
		, _vertexBuffer(VK_NULL_HANDLE)
		, _indexBuffer(VK_NULL_HANDLE)
		, _uniformBuffer(VK_NULL_HANDLE)
		, _textureImage(VK_NULL_HANDLE)
		, _textureImageView(VK_NULL_HANDLE)
		, _textureSampler(VK_NULL_HANDLE)
		, _bMinimized(false)
	{
		JE_AssertThrow(HelloTriangle::_singletonInstance == nullptr, "HelloTriangle can only have one instance!");
		HelloTriangle::_singletonInstance = this;
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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
	}

	void HelloTriangle::InitVulkan()
	{
		CreateInstance();
		SetupDebugCallback();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDeviceAndGetQueues();
		CreateCommandPool();
		CreateDescriptorPool();

		TextureInfo texInfo;
		CreateTextureImage(texInfo);
		CreateTextureImageView(texInfo);
		CreateTextureSampler(texInfo);

		LoadModel(MODEL_NAME_MESH, _modelInfo);
		CreateVertexBuffer();
		CreateIndexBuffer();
		UnloadModel(_modelInfo);

		CreateUniformBuffer();
		CreateDescriptorSetLayout();
		CreateDescriptorSet();
		CreatePushConstantRange();

		InitObjects();

		RecreateSwapChain();

		_camera.SetDimension(static_cast<float>(_swapChainExtent.width) / _swapChainExtent.height);
		_camera.Update();
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

		rating += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 6 : 0;
		rating += features.samplerAnisotropy ? 3 : 0;
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
		deviceFeatures.samplerAnisotropy = VK_TRUE;
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
		if (_swapChain != VK_NULL_HANDLE)
			return;

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

		TextureInfo dummy;

		for (size_t i = 0; i < _swapChainImages.size(); ++i)
		{
			_swapChainImageViews[i] = CreateImageView(dummy, _swapChainImages[i], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void HelloTriangle::InitObjects()
	{
		glm::vec3 pos(0.0f, -2.2f, 1.2f);
		glm::vec3 tgt(0.0f, 0.0f, 0.3f);
		_camera.Initialize
		(
			&pos,
			&tgt,
			45.0f,
			static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
			0.5f,
			3.0f
		);

		glm::vec3 col(1.0f, 1.0f, 0.0f);
		glm::vec3 dir(-1.0f, 1.0f, -1.0f);
		dir = glm::normalize(dir);
		_lightDirectional.Initialize
		(
			&col,
			&dir
		);

		col.r = _clearColor.color.float32[0];
		col.g = _clearColor.color.float32[1];
		col.b = _clearColor.color.float32[2];
		_fog.Initialize(&col, 2.3f, 3.0f);
	}

	void HelloTriangle::RefreshCameraProj(uint32_t newWidth, uint32_t newHeight)
	{
		_camera.SetDimension(static_cast<float>(newWidth) / static_cast<float>(newHeight));
	}

	void HelloTriangle::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = _swapChainFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		JE_AssertThrowVkResult(vkCreateRenderPass(_device, &renderPassInfo, _pAllocator, &_renderPass));
	}

	void HelloTriangle::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // TODO: For textures probably.

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		JE_AssertThrowVkResult(vkCreateDescriptorSetLayout(_device, &layoutInfo, _pAllocator, &_descriptorSetLayout));
	}

	void HelloTriangle::CreateGraphicsPipeline()
	{
		// Shader modules.

		std::vector<uint8_t> vertShaderData, fragShaderData;

		LoadShader("TutorialShader", ShaderType::Vertex, vertShaderData);
		LoadShader("TutorialShader", ShaderType::Fragment, fragShaderData);

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderData);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderData);

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};

		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";	// It's possible to combine multiple fragment shaders into a single shader module and use different entry points to differentiate between their behaviors. 
		shaderStages[0].pSpecializationInfo = nullptr; //  It allows you to specify values for shader constants. You can use a single shader module where its behavior can be configured at pipeline creation by specifying different values for the constants used in it. 

		shaderStages[1] = shaderStages[0];
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;

		
		// Vertex input state creation info.

		VkVertexInputBindingDescription bindingDescription;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		VertexTutorial::GetBindingDescription(bindingDescription);
		VertexTutorial::GetAttributeDescription(attributeDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		
		// Input assembly creation info.

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	// TODO: check what with adjacency means
		inputAssembly.primitiveRestartEnable = VK_FALSE;


		// Viewport creation info.

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_swapChainExtent.width);
		viewport.height = static_cast<float>(_swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = _swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;


		// Rasterizer state creation info.

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;	// This is useful in some special cases like shadow maps. 
		rasterizer.rasterizerDiscardEnable = VK_FALSE; // If is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;


		// Multisampling info.

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;


		// Depth and stencil info.

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};


		// Color blend attachment state (configuration per attached framebuffer).

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};

		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


		// Color blend state create info.

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;	// If set to true, this overwrites attachment state settings.
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;


		// Dynamic state info (states which you want to change in runtime).

		// Just for testin'.

		VkDynamicState dynamicStates[] = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;


		// Pipeline layout create info.

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &_pushConstantRange;

		JE_AssertThrowVkResult(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, _pAllocator, &_pipelineLayout));


		// Finally create graphics pipeline, yay.

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		//pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT; // You can use this to derive from existing pipeline which is faster than creating one from scratch.

		JE_AssertThrowVkResult(vkCreateGraphicsPipelines(_device, nullptr, 1, &pipelineInfo, _pAllocator, &_graphicsPipeline));

		// Shader modules cleanup.

		vkDestroyShaderModule(_device, vertShaderModule, _pAllocator);
		vkDestroyShaderModule(_device, fragShaderModule, _pAllocator);
	}

	VkShaderModule HelloTriangle::CreateShaderModule(const std::vector<uint8_t> code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		JE_AssertThrowVkResult(vkCreateShaderModule(_device, &createInfo, _pAllocator, &shaderModule));
		return shaderModule;
	}

	void HelloTriangle::CreateFramebuffers()
	{
		_swapChainFramebuffers.resize(_swapChainImageViews.size());

		for (size_t i = 0; i < _swapChainImageViews.size(); ++i)
		{
			std::array<VkImageView, 2> attachments = 
			{
				_swapChainImageViews[i],
				_depthImageView
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = _swapChainExtent.width;
			framebufferInfo.height = _swapChainExtent.height;
			framebufferInfo.layers = 1;

			JE_AssertThrowVkResult(vkCreateFramebuffer(_device, &framebufferInfo, _pAllocator, &_swapChainFramebuffers[i]));
		}
	}

	void HelloTriangle::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices;
		FindQueueFamilies(_physicalDevice, queueFamilyIndices);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;
		poolInfo.flags = 0;

		JE_AssertThrowVkResult(vkCreateCommandPool(_device, &poolInfo, _pAllocator, &_commandPool));

		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		JE_AssertThrowVkResult(vkCreateCommandPool(_device, &poolInfo, _pAllocator, &_commandPoolTransient));
	}

	void HelloTriangle::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		JE_AssertThrowVkResult(vkCreateDescriptorPool(_device, &poolInfo, _pAllocator, &_descriptorPool));
	}

	void HelloTriangle::CreateTextureImage(TextureInfo& outTexInfo)
	{
		LoadTexture(MODEL_NAME_TEXTURE, 4, outTexInfo);
		JE_Assert(outTexInfo.Data != nullptr);

		VkDeviceSize imageSize = outTexInfo.Width * outTexInfo.Height * outTexInfo.Channels;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<void*>(outTexInfo.Data), stagingBufferMemory, imageSize);

		UnloadTexture(outTexInfo);

		VkFormat texFormat = VK_FORMAT_R8G8B8A8_UNORM;

		CreateImage(outTexInfo, texFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);

		TransitionImageLayout(outTexInfo, _textureImage, texFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyBufferToImage(stagingBuffer, _textureImage, outTexInfo);

		GenerateMipmaps(outTexInfo, _textureImage);
		// This is now done in GenerateMipmaps.
		//TransitionImageLayout(outTexInfo, _textureImage, texFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); 

		vkDestroyBuffer(_device, stagingBuffer, _pAllocator);
		vkFreeMemory(_device, stagingBufferMemory, _pAllocator);
	}

	void HelloTriangle::CreateTextureImageView(const TextureInfo& texInfo)
	{
		_textureImageView = CreateImageView(texInfo, _textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void HelloTriangle::CreateTextureSampler(const TextureInfo& texInfo)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(texInfo.MipCount);

		JE_AssertThrowVkResult(vkCreateSampler(_device, &samplerInfo, _pAllocator, &_textureSampler));
	}

	void HelloTriangle::CreateDepthResources()
	{
		VkFormat depthFormat = FindDepthFormat();
		TextureInfo texInfo;
		texInfo.Width = _swapChainExtent.width;
		texInfo.Height = _swapChainExtent.height;
		CreateImage(texInfo, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
		_depthImageView = CreateImageView(texInfo, _depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		TransitionImageLayout(texInfo, _depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void HelloTriangle::CreateVertexBuffer()
	{
		JE_Assert(_modelInfo.IsLoaded());
#if USE_STAGING_BUFFER
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_modelInfo.Vertices);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_modelInfo.Vertices.data()), stagingBufferMemory, static_cast<size_t>(bufferSize));

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

		CopyBuffer_GPU_GPU(stagingBuffer, _vertexBuffer, bufferSize);

		vkDestroyBuffer(_device, stagingBuffer, _pAllocator);
		vkFreeMemory(_device, stagingBufferMemory, _pAllocator);
#else
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_modelInfo.Vertices);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _vertexBuffer, _vertexBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_modelInfo.Vertices.data()), _vertexBufferMemory, static_cast<size_t>(bufferSize));
#endif
	}

	void HelloTriangle::CreateIndexBuffer()
	{
		JE_Assert(_modelInfo.IsLoaded());
#if USE_STAGING_BUFFER
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_modelInfo.Indices);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_modelInfo.Indices.data()), stagingBufferMemory, static_cast<size_t>(bufferSize));

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

		CopyBuffer_GPU_GPU(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(_device, stagingBuffer, _pAllocator);
		vkFreeMemory(_device, stagingBufferMemory, _pAllocator);
#else
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_modelInfo.Indices);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _indexBuffer, _indexBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_modelInfo.Indices.data()), _indexBufferMemory, static_cast<size_t>(bufferSize));
#endif
	}

	void HelloTriangle::CreateUniformBuffer()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffer, _uniformBufferMemory);
	}

	void HelloTriangle::CreateCommandBuffers()
	{
		// Because one of the drawing commands involves binding the right VkFramebuffer, we'll actually have to record a command buffer for every image in the swap chain once again.

		_commandBuffers.resize(_swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		JE_AssertThrowVkResult(vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()));

		for (size_t i = 0; i < _commandBuffers.size(); i++) 
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			JE_AssertThrowVkResult(vkBeginCommandBuffer(_commandBuffers[i], &beginInfo));

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0] = _clearColor;
			clearValues[1] = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderPass;
			renderPassInfo.framebuffer = _swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = _swapChainExtent;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

			PushConstantObject pco;
			pco.FogColor = *_fog.GetColor();
			pco.FogDepthNear = _fog.GetStartDepth();
			pco.FogDepthFar = _fog.GetEndDepth();
			pco.LightColor = *_lightDirectional.GetColor();
			pco.LightDirectionV = *_lightDirectional.GetDirectionV();
			vkCmdPushConstants(_commandBuffers[i], _pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantObject), &pco);

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
			vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, &_vertexBuffer, offsets);
			vkCmdBindIndexBuffer(_commandBuffers[i], _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(_modelInfo.IndexCount), 1, 0, 0, 0);

			vkCmdEndRenderPass(_commandBuffers[i]);

			JE_AssertThrowVkResult(vkEndCommandBuffer(_commandBuffers[i]));
		}
	}

	void HelloTriangle::CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_descriptorSetLayout;

		JE_AssertThrowVkResult(vkAllocateDescriptorSets(_device, &allocInfo, &_descriptorSet));

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = _uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject); // If you're overwriting the whole buffer, like we are in this case, then it is is also possible to use the VK_WHOLE_SIZE value for the range. 

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = _textureImageView;
		imageInfo.sampler = _textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = _descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = _descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
 	}

	void HelloTriangle::CreatePushConstantRange()
	{
		_pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		_pushConstantRange.offset = 0;
		_pushConstantRange.size = sizeof(PushConstantObject);
	}

	void HelloTriangle::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		_semsImageAvailable.resize(MAX_FRAMES_IN_FLIGHT);
		_semsRenderFinished.resize(MAX_FRAMES_IN_FLIGHT);
		_fencesInFlight.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			JE_AssertThrowVkResult(vkCreateSemaphore(_device, &semaphoreInfo, _pAllocator, &_semsImageAvailable[i]));
			JE_AssertThrowVkResult(vkCreateSemaphore(_device, &semaphoreInfo, _pAllocator, &_semsRenderFinished[i]));
			JE_AssertThrowVkResult(vkCreateFence(_device, &fenceInfo, _pAllocator, &_fencesInFlight[i]));
		}
	}

	void HelloTriangle::MainLoop()
	{
		while (!glfwWindowShouldClose(_pWindow))
		{
			glfwPollEvents();
			if (!_bMinimized)
			{
				UpdateObjects();
				UpdateUniformBuffer();
				DrawFrame();
			}
			else
			{
				CheckForMinimized();
				if (!_bMinimized)
				{
					RecreateSwapChain();
				}
			}
		}

		vkDeviceWaitIdle(_device);
	}

#include "Rendering/SystemDrawable.h"

	void HelloTriangle::UpdateObjects()
	{
		Rendering::SystemDrawable* sys = Rendering::SystemDrawable::GetInstance();

		_fog.Update();
		_lightDirectional.Update();
		_camera.Update();
	}

	void HelloTriangle::DrawFrame()
	{
		vkWaitForFences(_device, 1, &_fencesInFlight[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(_device, 1, &_fencesInFlight[_currentFrame]);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint64_t>::max(), _semsImageAvailable[_currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			CheckForMinimized();
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			JE_AssertThrowVkResult(result);
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &_semsImageAvailable[_currentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semsRenderFinished[_currentFrame];

		JE_AssertThrowVkResult(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _fencesInFlight[_currentFrame]));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &_semsRenderFinished[_currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapChain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		
		result = vkQueuePresentKHR(_presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			CheckForMinimized();
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			JE_AssertThrowVkResult(result);
		}

		// vkQueueWaitIdle(_presentQueue); // Do not use it because this prevents only one queue being processed at a time.


		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void HelloTriangle::CheckForMinimized()
	{
		VkSurfaceCapabilitiesKHR capabilities;
		JE_AssertThrowVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &capabilities));
		
		if (capabilities.currentExtent.width == 0 || capabilities.currentExtent.height == 0)
		{
			_bMinimized = true;
		}
		else
		{
			RefreshCameraProj(capabilities.currentExtent.width, capabilities.currentExtent.height);
			_bMinimized = false;
		}
	}

	void HelloTriangle::UpdateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::mat4 mv_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mv_translation = mv_translation * rotation * scale;

		_ubo.MVP = *_camera.GetViewProj() * mv_translation;
		_ubo.MV = mv_translation;
		_ubo.MVInverseTranspose = glm::transpose(glm::inverse(mv_translation));

		CopyBuffer_CPU_GPU(reinterpret_cast<void*>(&_ubo), _uniformBufferMemory, sizeof(_ubo));
	}

	VkCommandBuffer HelloTriangle::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPoolTransient;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		JE_AssertThrowVkResult(vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer));

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void HelloTriangle::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		JE_AssertThrowVkResult(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		JE_AssertThrowVkResult(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
		JE_AssertThrowVkResult(vkQueueWaitIdle(_graphicsQueue));

		vkFreeCommandBuffers(_device, _commandPoolTransient, 1, &commandBuffer);
	}

	void HelloTriangle::TransitionImageLayout(const TextureInfo& texInfo, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = static_cast<uint32_t>(texInfo.MipCount);
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage, destinationStage;
		if (
			oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			JE_AssertThrow(false, "Unsupported layout transition!");
		}

		vkCmdPipelineBarrier
		(
			commandBuffer,
			sourceStage,
			destinationStage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		EndSingleTimeCommands(commandBuffer);
	}

	void HelloTriangle::GenerateMipmaps(const TextureInfo & texInfo, VkImage image)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texInfo.Width;
		int32_t mipHeight = texInfo.Height;

		for (uint32_t i = 1; i < static_cast<uint32_t>(texInfo.MipCount); ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier
			(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier
			);

			const int32_t nextMipWidth = mipWidth / 2;
			const int32_t nextMipHeight = mipHeight / 2;

			VkImageBlit blit = {};

			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { nextMipWidth, nextMipHeight, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier
			(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier
			);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = static_cast<uint32_t>(texInfo.MipCount) - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier
		(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		EndSingleTimeCommands(commandBuffer);
	}

	VkFormat HelloTriangle::FindSupportedFormat(const std::vector<VkFormat>& candidateFormats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
	{
		for (VkFormat format : candidateFormats)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

			if
			(
				tiling == VK_IMAGE_TILING_LINEAR 
				&& (props.linearTilingFeatures & featureFlags) == featureFlags
			)
			{
				return format;
			}
			else if 
			(
				tiling == VK_IMAGE_TILING_OPTIMAL
				&& (props.optimalTilingFeatures & featureFlags) == featureFlags
			)
			{
				return format;
			}
		}

		JE_AssertThrow(false, "Failed to find any supported format!");
	}

	VkFormat HelloTriangle::FindDepthFormat()
	{
		return FindSupportedFormat
		(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool HelloTriangle::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void HelloTriangle::RecreateSwapChain()
	{
		vkDeviceWaitIdle(_device);

		CleanupSwapChain();

		if (!_bMinimized)
		{
			CreateSyncObjects();
			CreateSwapChain();
			RetrieveSwapChainImages();
			CreateSwapChainImageViews();
			CreateRenderPass();
			CreateGraphicsPipeline();
			CreateDepthResources();
			CreateFramebuffers();
			CreateCommandBuffers();
		}
	}

	uint32_t HelloTriangle::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			if (
				typeFilter & (1 << i)
				&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties
				)
			{
				return i;
			}
		}

		JE_AssertThrow(false, "Failed to find suitable memory type!");
	}

	void HelloTriangle::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & outBuffer, VkDeviceMemory & outBufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		JE_AssertThrowVkResult(vkCreateBuffer(_device, &bufferInfo, _pAllocator, &outBuffer));

		// Buffer memory allocation.

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, outBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		JE_AssertThrowVkResult(vkAllocateMemory(_device, &allocInfo, _pAllocator, &outBufferMemory));
		JE_AssertThrowVkResult(vkBindBufferMemory(_device, outBuffer, outBufferMemory, 0));
	}

	void HelloTriangle::CopyBuffer_CPU_GPU(const void * srcData, VkDeviceMemory dstMemory, size_t copySize)
	{
		void* mem;
		JE_AssertThrowVkResult(vkMapMemory(_device, dstMemory, 0, copySize, 0, &mem));
		memcpy(mem, srcData, copySize);
		vkUnmapMemory(_device, dstMemory);
	}

	void HelloTriangle::CopyBuffer_GPU_GPU(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize copySize)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = copySize;
		
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void HelloTriangle::CreateImage(const TextureInfo & texInfo, VkFormat format, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties, VkImage & outImage, VkDeviceMemory & outMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texInfo.Width);
		imageInfo.extent.height = static_cast<uint32_t>(texInfo.Height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = static_cast<uint32_t>(texInfo.MipCount);
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = initialLayout;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		JE_AssertThrowVkResult(vkCreateImage(_device, &imageInfo, _pAllocator, &outImage));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, outImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memProperties);

		JE_AssertThrowVkResult(vkAllocateMemory(_device, &allocInfo, _pAllocator, &outMemory));

		JE_AssertThrowVkResult(vkBindImageMemory(_device, outImage, outMemory, 0));
	}

	void HelloTriangle::CopyBufferToImage(VkBuffer buffer, VkImage image, const TextureInfo & texInfo)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent =
		{
			static_cast<uint32_t>(texInfo.Width),
			static_cast<uint32_t>(texInfo.Height),
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommands(commandBuffer);
	}

	VkImageView HelloTriangle::CreateImageView(const TextureInfo& texInfo, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = static_cast<uint32_t>(texInfo.MipCount);
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		VkImageView imageView;
		JE_AssertThrowVkResult(vkCreateImageView(_device, &viewInfo, _pAllocator, &imageView));
		return imageView;
	}

	void HelloTriangle::Cleanup()
	{
		CleanupObjects();

		vkDestroySampler(_device, _textureSampler, _pAllocator);
		_textureSampler = VK_NULL_HANDLE;
		vkDestroyImageView(_device, _textureImageView, _pAllocator);
		_textureImageView = VK_NULL_HANDLE;

		vkDestroyImage(_device, _textureImage, _pAllocator);
		_textureImage = VK_NULL_HANDLE;
		vkFreeMemory(_device, _textureImageMemory, _pAllocator);
		_textureImageMemory = VK_NULL_HANDLE;

		vkDestroyBuffer(_device, _uniformBuffer, _pAllocator);
		_uniformBuffer = VK_NULL_HANDLE;
		vkFreeMemory(_device, _uniformBufferMemory, _pAllocator);
		_uniformBufferMemory = VK_NULL_HANDLE;

		vkDestroyBuffer(_device, _indexBuffer, _pAllocator);
		_indexBuffer = VK_NULL_HANDLE;
		vkFreeMemory(_device, _indexBufferMemory, _pAllocator);
		_indexBufferMemory = VK_NULL_HANDLE;

		vkDestroyBuffer(_device, _vertexBuffer, _pAllocator);
		_vertexBuffer = VK_NULL_HANDLE;
		vkFreeMemory(_device, _vertexBufferMemory, _pAllocator);
		_vertexBufferMemory = VK_NULL_HANDLE;

		CleanupSwapChain();

		vkDestroyDescriptorPool(_device, _descriptorPool, _pAllocator);
		_descriptorPool = VK_NULL_HANDLE;

		vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, _pAllocator);
		_descriptorSetLayout = VK_NULL_HANDLE;

		vkDestroyCommandPool(_device, _commandPoolTransient, _pAllocator);
		_commandPoolTransient = VK_NULL_HANDLE;
		vkDestroyCommandPool(_device, _commandPool, _pAllocator);
		_commandPool = VK_NULL_HANDLE;

		vkDestroyDevice(_device, _pAllocator);
		_device = VK_NULL_HANDLE;

		CleanupDebugCallback();

		vkDestroySurfaceKHR(_instance, _surface, _pAllocator);
		_surface = VK_NULL_HANDLE;

		vkDestroyInstance(_instance, _pAllocator);
		_instance = VK_NULL_HANDLE;

		glfwDestroyWindow(_pWindow);
		glfwTerminate();
	}

	void HelloTriangle::CleanupObjects()
	{
		_camera.Shutdown();
		_lightDirectional.Shutdown();
		_fog.Shutdown();
	}

	void HelloTriangle::CleanupDebugCallback()
	{
		if (!_bEnableValidationLayers)
			return;

		CallVkProc(vkDestroyDebugReportCallbackEXT, _instance, _debugCallback, _pAllocator);
	}

	void HelloTriangle::CleanupSwapChain()
	{
		if (_renderPass == VK_NULL_HANDLE)
			return;

		vkDestroyImageView(_device, _depthImageView, _pAllocator);
		_depthImageView = VK_NULL_HANDLE;
		vkDestroyImage(_device, _depthImage, _pAllocator);
		_depthImage = VK_NULL_HANDLE;
		vkFreeMemory(_device, _depthImageMemory, _pAllocator);
		_depthImageMemory = VK_NULL_HANDLE;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyFence(_device, _fencesInFlight[i], _pAllocator);
			vkDestroySemaphore(_device, _semsImageAvailable[i], _pAllocator);
			vkDestroySemaphore(_device, _semsRenderFinished[i], _pAllocator);
		}

		vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();

		for (auto framebuffer : _swapChainFramebuffers)
		{
			vkDestroyFramebuffer(_device, framebuffer, _pAllocator);
		}
		_swapChainFramebuffers.clear();

		vkDestroyPipeline(_device, _graphicsPipeline, _pAllocator);
		_graphicsPipeline = VK_NULL_HANDLE;

		vkDestroyPipelineLayout(_device, _pipelineLayout, _pAllocator);
		_pipelineLayout = VK_NULL_HANDLE;

		vkDestroyRenderPass(_device, _renderPass, _pAllocator);
		_renderPass = VK_NULL_HANDLE;

		for (auto imageView : _swapChainImageViews)
		{
			vkDestroyImageView(_device, imageView, _pAllocator);
		}
		_swapChainImageViews.clear();

		if (!_bMinimized)
		{
			vkDestroySwapchainKHR(_device, _swapChain, _pAllocator);
			_swapChain = VK_NULL_HANDLE;
		}
	}

	void HelloTriangle::LoadFile(const std::string & fileName, std::vector<uint8_t>& outData)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		JE_AssertThrow(file.is_open(), "Failed to open requested file!");

		// (ate) The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer

		size_t fileSize = static_cast<size_t>(file.tellg());
		outData.clear();
		outData.resize(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(outData.data()), fileSize);

		file.close();
	}

	void HelloTriangle::LoadShader(const std::string& shaderName, ShaderType shaderType, std::vector<uint8_t>& outData)
	{
		LoadFile(RESOURCE_PATH + "Shaders\\Binary\\" + shaderName + ShaderTypeToExtension[shaderType] + ".spv", outData);
	}

	void HelloTriangle::LoadTexture(const std::string & textureName, uint32_t desiredChannels, TextureInfo& outTextureInfo)
	{
		int width, height, chnls;
		outTextureInfo.Data = stbi_load
		(
			(RESOURCE_PATH + "Textures\\Source\\" + textureName).c_str(),
			&width,
			&height,
			&chnls,
			desiredChannels
		);

		outTextureInfo.Width = width;
		outTextureInfo.Height = height;
		outTextureInfo.Channels = desiredChannels;
		outTextureInfo.SizeBytes = width * height * outTextureInfo.Channels;
		outTextureInfo.bAllocatedByStbi = true;

		outTextureInfo.MipCount = static_cast<uint8_t>(std::floor(std::log2(std::max(width, height)))) + 1;
	}

	void HelloTriangle::UnloadTexture(TextureInfo& texInfo)
	{
		if (texInfo.bAllocatedByStbi)
		{
			stbi_image_free(texInfo.Data);
		}
		else
		{
			free(texInfo.Data);
		}

		texInfo.Data = nullptr;
	}

	void HelloTriangle::LoadModel(const std::string& modelName, ModelInfo & outModelInfo)
	{
		JE_Assert(!outModelInfo.IsLoaded());

		const std::string finalPath = (RESOURCE_PATH + "Meshes\\Source\\" + modelName);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		JE_AssertThrow(tinyobj::LoadObj(&attrib, &shapes, &materials, &err, finalPath.c_str()), err);

		glm::vec3 dummyNormal = glm::vec3(0.0f, 0.0f, 1.0f);
		const bool bIncludeNormals = attrib.normals.size();

		std::unordered_map<VertexTutorial, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				VertexTutorial vertex = {};

				vertex.Position = 
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.Color = glm::vec3(1.0f, 1.0f, 1.0f);

				if (bIncludeNormals)
				{
					vertex.Normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				else
				{
					vertex.Normal = dummyNormal;
				}

				vertex.Uv =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(outModelInfo.Vertices.size());
					outModelInfo.Vertices.push_back(vertex);
				}

				outModelInfo.Indices.push_back(uniqueVertices[vertex]);
			}
		}

		outModelInfo.IndexCount = static_cast<uint32_t>(outModelInfo.Indices.size());
	}

	void HelloTriangle::UnloadModel(ModelInfo & modelInfo)
	{
		JE_Assert(modelInfo.IsLoaded());

		modelInfo.Vertices.clear();
		modelInfo.Indices.clear();
	}

	void HelloTriangle::VertexTutorial::GetBindingDescription(VkVertexInputBindingDescription& outDescription)
	{
		outDescription.binding = 0;
		outDescription.stride = sizeof(VertexTutorial);
		outDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	void HelloTriangle::VertexTutorial::GetAttributeDescription(std::vector<VkVertexInputAttributeDescription>& outDescriptions)
	{
		outDescriptions.resize(COMPONENT_NUMBER);

		outDescriptions[0].binding = 0;
		outDescriptions[0].location = 0;
		outDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		outDescriptions[0].offset = offsetof(VertexTutorial, Position);

		outDescriptions[1].binding = 0;
		outDescriptions[1].location = 1;
		outDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		outDescriptions[1].offset = offsetof(VertexTutorial, Color);

		outDescriptions[2].binding = 0;
		outDescriptions[2].location = 2;
		outDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		outDescriptions[2].offset = offsetof(VertexTutorial, Normal);

		outDescriptions[3].binding = 0;
		outDescriptions[3].location = 3;
		outDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		outDescriptions[3].offset = offsetof(VertexTutorial, Uv);
	}
}