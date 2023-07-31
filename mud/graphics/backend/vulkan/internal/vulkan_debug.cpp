#include "vulkan_debug.hpp"

#include <fmt/format.h>

#include "utils/logger.hpp"

namespace mud::graphics_backend::vk
{
	std::string getVkResultString(VkResult vkr, bool prependCode)
    {
        std::string message = "VkResult string undefined";

		switch (vkr)
		{
			// Success codes
			case VK_SUCCESS:												message = "Command successfully completed";
			case VK_NOT_READY:												message = "A fence or query has not yet completed";
			case VK_TIMEOUT:												message = "A wait operation has not completed in the specified time";
			case VK_EVENT_SET:												message = "An event is signaled";
			case VK_EVENT_RESET:											message = "An event is unsignaled";
			case VK_INCOMPLETE:												message = "A return array was too small for the result";
			case VK_SUBOPTIMAL_KHR:											message = "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully";
			case VK_THREAD_IDLE_KHR:										message = "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call";
			case VK_THREAD_DONE_KHR:										message = "A deferred operation is not complete but there is no work remaining to assign to additional threads";
			case VK_OPERATION_DEFERRED_KHR:									message = "A deferred operation was requested and at least some of the work was deferred";
			case VK_OPERATION_NOT_DEFERRED_KHR:								message = "A deferred operation was requested and no operations were deferred";
			case VK_PIPELINE_COMPILE_REQUIRED:								message = "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed";

			// Error codes
			case VK_ERROR_OUT_OF_HOST_MEMORY:								message = "A host memory allocation has failed";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:								message = "A device memory allocation has failed";
			case VK_ERROR_INITIALIZATION_FAILED:							message = "Initialization of an object could not be completed for implementation-specific reasons";
			case VK_ERROR_DEVICE_LOST:										message = "The logical or physical device has been lost";
			case VK_ERROR_MEMORY_MAP_FAILED:								message = "Mapping of a memory object has failed";
			case VK_ERROR_LAYER_NOT_PRESENT:								message = "A requested layer is not present or could not be loaded";
			case VK_ERROR_EXTENSION_NOT_PRESENT:							message = "A requested extension is not supported";
			case VK_ERROR_FEATURE_NOT_PRESENT:								message = "A requested feature is not supported";
			case VK_ERROR_INCOMPATIBLE_DRIVER:								message = "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";
			case VK_ERROR_TOO_MANY_OBJECTS:									message = "Too many objects of the type have already been created";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:								message = "A requested format is not supported on this device";
			case VK_ERROR_FRAGMENTED_POOL:									message = "A pool allocation has failed due to fragmentation of the pool's memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation";
			case VK_ERROR_SURFACE_LOST_KHR:									message = "A surface is no longer available";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:							message = "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again";
			case VK_ERROR_OUT_OF_DATE_KHR:									message = "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:							message = "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image";
			case VK_ERROR_INVALID_SHADER_NV:								message = "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled";
			case VK_ERROR_OUT_OF_POOL_MEMORY:								message = "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:							message = "An external handle is not a valid handle of the specified type";
			case VK_ERROR_FRAGMENTATION:									message = "A descriptor pool creation has failed due to fragmentation";
			//case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:						message = "A buffer creation failed because the requested address is not available";
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:					message = "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid";
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:				message = "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exclusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application's control";
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:						message = "An image creation failed because internal resources required for compression are exhausted. This must only be returned when fixed-rate compression is requested";
			//case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:					message = "The requested VkImageUsageFlags are not supported";
			//case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:			message = "The requested video picture layout is not supported";
			//case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:		message = "A video profile operation specified via VkVideoProfileKHR::videoCodecOperation is not supported";
			//case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:			message = "Format parameters in a requested VkVideoProfileKHR chain are not supported";
			//case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:			message = "Codec-specific parameters in a requested VkVideoProfileKHR chain are not supported";
			//case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:			message = "The specified video Std header version is not supported";
			case VK_ERROR_UNKNOWN:											message = "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred";
		}

		if (prependCode)
		{
			return fmt::format("[{0}] {1}", (int)vkr, message);
		}

        return message;
    }

    bool checkVkResult(VkResult result, const std::string & errorMessage)
	{
		if (result == VK_SUCCESS)
			return true;

		log(LogLevel::Error, fmt::format("{0}: {1}\n", errorMessage, getVkResultString(result)), "Vulkan");
		return false;
	}
}