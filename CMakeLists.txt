cmake_minimum_required(VERSION 3.13.0)
project(mud VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)

include(CTest)
enable_testing()

add_executable(mud mud/main.cpp)
target_sources(mud PRIVATE
	mud/application.cpp
	mud/scene.cpp)
add_subdirectory(mud/game)
add_subdirectory(mud/graphics)
add_subdirectory(mud/math)
add_subdirectory(mud/platform)
add_subdirectory(mud/utils)

target_include_directories(mud PRIVATE mud/dependencies/include mud/)
target_link_directories(mud PRIVATE mud/dependencies/lib)
target_link_libraries(mud libglfw3.a vulkan-1.lib shaderc_shared.lib libspirv-cross.a libassimp.a libzlibstatic.a libfreetype.a)

target_compile_features(mud PRIVATE cxx_std_17)

target_compile_definitions(mud PRIVATE
	MUD_USE_VULKAN)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)