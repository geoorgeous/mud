#include "vulkan_context.hpp"

#include <glfw/glfw3.h>

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"

namespace mud::graphics_backend::vk
{
	VulkanContext::~VulkanContext()
	{
#ifdef MUD_ENABLE_VULKAN_DEBUG
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(m_vkInstance, m_debugMessenger, nullptr);
		}
#endif

		vkDestroyInstance(m_vkInstance, nullptr);
	}

	void VulkanContext::init(const std::vector<std::string> & layers)
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Mud Demo";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Mud";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

		std::vector<std::string> instanceExtensions;

		uint32_t glfwExtensionCount = 0;
		const char ** glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (size_t idx = 0; idx < glfwExtensionCount; ++idx)
			instanceExtensions.emplace_back(glfwInstanceExtensions[idx]);

#ifdef MUD_ENABLE_VULKAN_DEBUG

		instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::vector<std::string> availableRequestedLayers;

		for (const std::string & layer : layers)
		{
			bool layerFound = false;

			for (const auto & layerProperties : availableLayers)
			{
				if (layer == layerProperties.layerName)
				{
					layerFound = true;
					availableRequestedLayers.push_back(layer);
					break;
				}
			}

			if (!layerFound)
			{
				log(LogLevel::Error, fmt::format("Validation layer '{0}' not available\n", layer), "Vulkan");
			}
		}

		std::vector<const char *> availableRequestedLayersCStrs;
		for (const std::string & availableRequestedLayer : availableRequestedLayers)
			availableRequestedLayersCStrs.push_back(availableRequestedLayer.data());

		createInfo.enabledLayerCount = static_cast<uint32_t>(availableRequestedLayersCStrs.size());
		createInfo.ppEnabledLayerNames = availableRequestedLayersCStrs.data();

		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;
		// createInfo.pUserData = nullptr;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugMessengerCreateInfo;

#endif

		std::vector<const char *> instanceExtensionsCStrs;
		for (const std::string & instanceExtension : instanceExtensions)
			instanceExtensionsCStrs.push_back(instanceExtension.data());

		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionsCStrs.size());
		createInfo.ppEnabledExtensionNames = instanceExtensionsCStrs.data();

		if (!MUD__checkVulkanCall(vkCreateInstance(&createInfo, nullptr, &m_vkInstance), "Failed to create Vulkan instance"))
			return;

#ifdef MUD_ENABLE_VULKAN_DEBUG
		initDebugUtilsMessenger();
#endif
	}

	VkInstance VulkanContext::getVulkanHandle() const
	{
		return m_vkInstance;
	}

#ifdef MUD_ENABLE_VULKAN_DEBUG

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		LogLevel level;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			level = LogLevel::Trace;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			level = LogLevel::Info;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			level = LogLevel::Warning;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			level = LogLevel::Error;
			break;
		}

		std::string messageTypeString;
		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			messageTypeString = "General";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			messageTypeString = "Validation";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			messageTypeString = "Performance";
			break;
		}

		log(level, fmt::format("{0} - {1}\n", messageTypeString, pCallbackData->pMessage), "Vulkan");

		return VK_FALSE;
	}

	void VulkanContext::initDebugUtilsMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugUtilsMessengerCallback;
		// createInfo.pUserData = nullptr;

		MUD__checkVulkanCall(createDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_debugMessenger), "Failed to setup debug messenger");
	}

#endif
}