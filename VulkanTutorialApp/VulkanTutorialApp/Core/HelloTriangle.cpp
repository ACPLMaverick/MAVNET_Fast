#include "HelloTriangle.h"

#include <fstream>

#include "Rendering/Helper.h"

#include "Rendering/resource/buffer/UboCommon.h" // TODO: temp

#include "GOM/system/Drawable.h"
#include "GOM/system/Transform.h"
#include "GOM/system/Rotator.h"

namespace Core
{
	const std::string HelloTriangle::RESOURCE_PATH = "..\\..\\JadeEngine\\JadeEngine\\Resources\\";

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
		, _commandPoolDynamic(VK_NULL_HANDLE)
		, _commandPoolStatic(VK_NULL_HANDLE)
		, _commandPoolTransient(VK_NULL_HANDLE)
		, _currentFrame(0)
		, _imageIndex(0)
		, _activeRenderStep(nullptr)
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
		InitObjects();	// TODO: Transfer this to GOM...
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
		//
		CreateInstance();
		SetupDebugCallback();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDeviceAndGetQueues();
		CreateCommandPool();

		CreateSyncObjects();	// Only once.
		CreateSwapChain();
		CreateSwapChainAttachments();
		//

		// CreatePushConstantRange(); // TODO: Delegate elsewhere.

		::Rendering::Helper::GetInstance()->Initialize();

		_uidMgr.Initialize();
		_samplerMgr.Initialize();
		_descriptorMgr.Initialize();
		_pipelineMgr.Initialize();
		_renderPassMgr.Initialize();

		_renderStepCache.Initialize();

		_resourceManager.Initialize();

		CreateCommandBuffers();
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
		appInfo.apiVersion = VK_API_VERSION_1_1;

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

	void HelloTriangle::CreateSwapChainAttachments()
	{
		Rendering::Attachment::CreateAttachmentsFromSwapchain(_swapChainAttachments);
	}

	void HelloTriangle::InitObjects()
	{
		_system.Initialize();

		// TODO: Camera should be an entity component, with an ability to render to a specific RenderTarget with a given RenderPass, etc.
		glm::vec3 pos(-3.0f, 1.5f, -3.0f);
		const float len = 9.0f;
		pos.x *= len;
		pos.z *= len;
		glm::vec3 tgt(0.0f, 0.0f, 0.0f);
		_camera.Initialize
		(
			&pos,
			&tgt,
			45.0f,
			static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
			0.5f,
			120.0f
		);
		_camera.SetDimension(static_cast<float>(_swapChainExtent.width) / _swapChainExtent.height);
		_camera.Update();

		_world.Initialize();

		std::vector<::Rendering::Material*> availableMaterials;
		availableMaterials.push_back(JE_GetApp()->GetResourceManager()->CacheMaterials.Get("TutorialMaterial"));

		std::vector<::Rendering::Mesh*> availableMeshes;
		availableMeshes.push_back(JE_GetApp()->GetResourceManager()->CacheMeshes.Get("AutoGen_Box"));
		availableMeshes.push_back(JE_GetApp()->GetResourceManager()->CacheMeshes.Get("AutoGen_Sphere"));
		availableMeshes.push_back(JE_GetApp()->GetResourceManager()->CacheMeshes.Get("AutoGen_Cylinder"));

		const size_t materialNum = availableMaterials.size();
		const size_t meshNum = availableMeshes.size();

		const size_t objNumX = 20;
		const size_t objNumZ = 20;
		const float objSpacing = 2.0f;

		float baseX = -(float)(objNumX / 2) * objSpacing;

		for (size_t i = 0; i < objNumX; ++i)
		{
			float baseZ = -(float)(objNumZ / 2) * objSpacing;
			for (size_t j = 0; j < objNumZ; ++j)
			{
				GOM::Entity* entity = _world.AddEntity();


				const glm::vec3 pos(baseX, 0.0f, baseZ);
				const glm::vec3 rot(0.0f, 0.0f, 0.0f);
				const glm::vec3 scl(1.0f, 1.0f, 1.0f);

				const bool bDoesMove = (bool)JE_GetApp()->GetRandom()->Get32(0, 1);
				GOM::TransfromConstructionParameters dynamicParam;
				dynamicParam.InitMovability = bDoesMove ? GOM::Transform::Movability::Dynamic : GOM::Transform::Movability::Static;

				GOM::Transform* transform = static_cast<GOM::Transform*>(GOM::Transform::GetBehaviour()->ConstructComponent(&dynamicParam));
				transform->SetPosition(pos);
				transform->SetRotation(rot);
				transform->SetScale(scl);

				GOM::Transform::GetBehaviour()->InitializeComponent(transform, entity);

				
				const size_t currMatIndex = JE_GetApp()->GetRandom()->Get64(0, materialNum - 1);
				const size_t currMeshIndex = JE_GetApp()->GetRandom()->Get64(0, meshNum - 1);
				::GOM::Drawable* drawableComponent = static_cast<::GOM::Drawable*>(GOM::Drawable::GetBehaviour()->ConstructComponent());
				JE_SetPropertyPtr(drawableComponent, PropMaterial, availableMaterials[currMatIndex]);
				JE_SetPropertyPtr(drawableComponent, PropMesh, availableMeshes[currMeshIndex]);

				GOM::Drawable::GetBehaviour()->InitializeComponent(drawableComponent, entity);


				if (bDoesMove)
				{
					const float rotationMin = 0.1f;
					const float rotationMax = 1.0f;
					const glm::vec3 rotatorValue
					(
						JE_GetApp()->GetRandom()->GetFloat(rotationMin, rotationMax),
						JE_GetApp()->GetRandom()->GetFloat(rotationMin, rotationMax),
						JE_GetApp()->GetRandom()->GetFloat(rotationMin, rotationMax)
					);

					GOM::Rotator* rotator = static_cast<GOM::Rotator*>(GOM::Rotator::GetBehaviour()->ConstructComponent());
					JE_SetPropertyPtr(rotator, PropRotation, rotatorValue);
					GOM::Rotator::GetBehaviour()->InitializeComponent(rotator, entity);
				}

				baseZ += objSpacing;
			}
			baseX += objSpacing;
		}
	}

	void HelloTriangle::RefreshCameraProj(uint32_t newWidth, uint32_t newHeight)
	{
		_camera.SetDimension(static_cast<float>(newWidth) / static_cast<float>(newHeight));
	}

	/*
	void HelloTriangle::CreateRenderPass()
	{
		JE_Assert(false); // Legacy code.

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

		//JE_AssertThrowVkResult(vkCreateRenderPass(_device, &renderPassInfo, _pAllocator, &_renderPass));
	}
	*/
	/*
	void HelloTriangle::CreateGraphicsPipeline()
	{
		JE_Assert(false); // Legacy code.

		// Shader modules.

		std::vector<uint8_t> vertShaderData, fragShaderData;

		Rendering::Shader shader;
		shader.Load("TutorialShader");

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		shader.CreatePipelineShaderStageInfos(&shaderStages);

		
		// Vertex input state creation info.

		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		_material->GetVertexDeclaration()->GetBindingDescriptions(&bindingDescriptions);
		_material->GetVertexDeclaration()->GetAttributeDescriptions(&attributeDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
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

		VkDescriptorSetLayout layouts[] = { _material->GetDescriptorSet()->GetAssociatedVkDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = layouts;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &_pushConstantRange;

		//JE_AssertThrowVkResult(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, _pAllocator, &_pipelineLayout));


		// Finally create graphics pipeline, yay.

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;

		//pipelineInfo.layout = _pipelineLayout;
		//pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		//pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT; // You can use this to derive from existing pipeline which is faster than creating one from scratch.

		//JE_AssertThrowVkResult(vkCreateGraphicsPipelines(_device, nullptr, 1, &pipelineInfo, _pAllocator, &_graphicsPipeline));

		// Shader modules cleanup.

		shader.Cleanup();
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

			framebufferInfo.renderPass = GetActiveRenderStep()->GetRenderPass()->GetVkRenderPass();

			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = _swapChainExtent.width;
			framebufferInfo.height = _swapChainExtent.height;
			framebufferInfo.layers = 1;

			JE_AssertThrowVkResult(vkCreateFramebuffer(_device, &framebufferInfo, _pAllocator, &_swapChainFramebuffers[i]));
		}
	}
	*/

	void HelloTriangle::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices;
		FindQueueFamilies(_physicalDevice, queueFamilyIndices);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;
		poolInfo.flags = 0;

		JE_AssertThrowVkResult(vkCreateCommandPool(_device, &poolInfo, _pAllocator, &_commandPoolStatic));

		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		JE_AssertThrowVkResult(vkCreateCommandPool(_device, &poolInfo, _pAllocator, &_commandPoolDynamic));

		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		JE_AssertThrowVkResult(vkCreateCommandPool(_device, &poolInfo, _pAllocator, &_commandPoolTransient));
	}

	void HelloTriangle::CreateCommandBuffers()
	{
		// Because one of the drawing commands involves binding the right VkFramebuffer, we'll actually have to record a command buffer for every image in the swap chain once again.

		_commandBuffers.resize(_swapChainAttachments.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = GetCommandPoolDynamic();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		JE_AssertThrowVkResult(vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()));
	}

	/*
	void HelloTriangle::CreatePushConstantRange()
	{
		_pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		_pushConstantRange.offset = 0;
		_pushConstantRange.size = sizeof(Rendering::UboCommon::SceneGlobal);
	}
	*/

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
				UpdateWindowStatusBar();
				UpdateObjects();
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

	void HelloTriangle::UpdateObjects()
	{
		_timer.UpdatePerFrame();

		_camera.Update();
		_world.Update();

		_system.Update();
	}

	void HelloTriangle::UpdateWindowStatusBar()
	{
		static const float UPDATE_PERIOD = 1.0f;
		static float updateTimer = 0.0f;
		static float fpsAccumulator = 0.0f;
		static int fpsNum = 0;

		const float dt = JE_GetApp()->GetGlobalTimer()->GetDt();
		const float fps = JE_GetApp()->GetGlobalTimer()->GetFPS();

		updateTimer += dt;
		fpsAccumulator += fps;
		++fpsNum;

		if (updateTimer >= UPDATE_PERIOD)
		{
			const float currFps = fpsAccumulator / (float)fpsNum;

			static const size_t BUF_SIZE = 128;
			static char buf[BUF_SIZE] = {};

			sprintf_s(buf, BUF_SIZE, "%s | [FPS: %f]", WINDOW_NAME, currFps);

			glfwSetWindowTitle(_pWindow, buf);

			updateTimer = 0.0f;
			fpsAccumulator = 0.0f;
			fpsNum = 0;
		}
	}

	void HelloTriangle::DrawFrame()
	{
		vkWaitForFences(_device, 1, &_fencesInFlight[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(_device, 1, &_fencesInFlight[_currentFrame]);

		VkResult result = vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint64_t>::max(), _semsImageAvailable[_currentFrame], VK_NULL_HANDLE, &_imageIndex);

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

		CreateCommandBuffer();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &_semsImageAvailable[_currentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffers[_imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semsRenderFinished[_currentFrame];

		JE_AssertThrowVkResult(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _fencesInFlight[_currentFrame]));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &_semsRenderFinished[_currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapChain;
		presentInfo.pImageIndices = &_imageIndex;
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

	void HelloTriangle::CreateCommandBuffer()
	{
		VkCommandBuffer cmd = GetCmd();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		JE_AssertThrowVkResult(vkBeginCommandBuffer(cmd, &beginInfo));

		// Perform all fixed render steps.
		// TODO: Consider behaviour for non-fixed render steps...

		for (size_t i = 0; i < (size_t)Rendering::RenderStepCommon::FixedId::ENUM_SIZE; ++i)
		{
			Rendering::RenderStep* step = _renderStepCache.Get((Rendering::RenderStepCommon::FixedId)i);
			if (step) // TODO Remove this if when all render steps are implemented.
			{
				_activeRenderStep = step;
				step->Perform();
			}
		}
		_activeRenderStep = nullptr;

		JE_AssertThrowVkResult(vkEndCommandBuffer(cmd));
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

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (_fencesInFlight[i] != nullptr)
			{
				vkDestroyFence(_device, _fencesInFlight[i], _pAllocator);
				_fencesInFlight[i] = nullptr;
			}
			if (_semsImageAvailable[i] != nullptr) 
			{
				vkDestroySemaphore(_device, _semsImageAvailable[i], _pAllocator);
				_semsImageAvailable[i] = nullptr;
			}
			if (_semsRenderFinished[i] != nullptr) 
			{
				vkDestroySemaphore(_device, _semsRenderFinished[i], _pAllocator);
				_semsRenderFinished[i] = nullptr;
			}
		}

		CleanupSwapChain();

		if(!_bMinimized)
		{
			CreateSwapChain();
			CreateSwapChainAttachments();
			CreateSyncObjects();

			_renderPassMgr.Reinitialize();
			_pipelineMgr.Reinitialize();
			
			_renderStepCache.OnSwapChainResize();

			_system.OnSwapChainResize();
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

	void HelloTriangle::Cleanup()
	{
		CleanupObjects();
		_world.Cleanup();
		_system.Cleanup();

		_resourceManager.Cleanup();

		_renderStepCache.Cleanup();

		_renderPassMgr.Cleanup();
		_pipelineMgr.Cleanup();
		_descriptorMgr.Cleanup();
		_samplerMgr.Cleanup();
		_uidMgr.Cleanup();

		// Other singleton instances destruction.
		::Rendering::Helper::GetInstance()->Cleanup();
		::Rendering::Helper::DestroyInstance();

		vkFreeCommandBuffers(_device, GetCommandPoolDynamic(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();

		CleanupSwapChain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyFence(_device, _fencesInFlight[i], _pAllocator);
			vkDestroySemaphore(_device, _semsImageAvailable[i], _pAllocator);
			vkDestroySemaphore(_device, _semsRenderFinished[i], _pAllocator);
		}

		vkDestroyCommandPool(_device, _commandPoolTransient, _pAllocator);
		_commandPoolTransient = VK_NULL_HANDLE;
		vkDestroyCommandPool(_device, _commandPoolDynamic, _pAllocator);
		_commandPoolDynamic = VK_NULL_HANDLE;
		vkDestroyCommandPool(_device, _commandPoolStatic, _pAllocator);
		_commandPoolStatic = VK_NULL_HANDLE;

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
	}

	void HelloTriangle::CleanupDebugCallback()
	{
		if (!_bEnableValidationLayers)
			return;

		CallVkProc(vkDestroyDebugReportCallbackEXT, _instance, _debugCallback, _pAllocator);
	}

	void HelloTriangle::CleanupSwapChain()
	{
		for (auto& attachment : _swapChainAttachments)
		{
			attachment->Cleanup();
			delete attachment;
		}
		_swapChainAttachments.clear();

		if (!_bMinimized)
		{
			vkDestroySwapchainKHR(_device, _swapChain, _pAllocator);
			_swapChain = VK_NULL_HANDLE;
		}
	}

	bool HelloTriangle::LoadFile(const std::string & fileName, std::vector<uint8_t>& outData)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			return false;

		// (ate) The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer

		size_t fileSize = static_cast<size_t>(file.tellg());
		outData.clear();
		outData.resize(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(outData.data()), fileSize);

		file.close();

		return true;
	}
}