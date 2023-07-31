#include "application.hpp"
#include "utils/logger.hpp"
#include "graphics/mesh.hpp"

#include <iostream>

#ifndef MUD_VERSION_STRING
#define MUD_VERSION_STRING ""
#endif

int main()
{
	try
    {
#ifdef NDEBUG
		mud::log(mud::LogLevel::Info, fmt::format("Mud Engine, {0}(Release) Copyright 2023 George McDonagh\n", MUD_VERSION_STRING" "));
#else
		mud::log(mud::LogLevel::Info, fmt::format("Mud Engine, {0}(Debug) Copyright 2023 George McDonagh\n", MUD_VERSION_STRING" "));
#endif

		mud::Application application;
		application.run();
	}
    catch (const std::exception& e)
    {
		mud::log(mud::LogLevel::Error, fmt::format("Fatal error: Unhandled exception thrown: {0}\n", e.what()));
		return EXIT_FAILURE;
    }
    catch (...)
    {
		mud::log(mud::LogLevel::Error, fmt::format("Fatal error: Unhandled exception thrown: Unknown\n"));
		return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}