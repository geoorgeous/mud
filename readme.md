#

<p align="right">
  <img src="./mud_logo.png?raw=true" alt="Sublime's custom image"/>
</p>

# Mud Engine

> [!IMPORTANT]
> I'm no longer actively working on this project. I'm working on a new engine project written in C which can be found [here](../cx)

This repo does not currently include the dependencies used for the project, or many of the resources that might be references in the source. See below for credits and download links.

## Features
- <b>Cross platform</b>: The engine is designed to be platform agnostic. This is achieved by using the [GLTF](https://www.glfw.org/) library to handle cross platform windowing and input, and by writing the rendering backend in [Vulkan](https://vkguide.dev/docs/introduction/vulkan_overview/) first and foremost.  
_See: [window.hpp](./mud/platform/window.hpp)_

- <b>Asset management</b>: Assets can be imported and managed via the [`AssetManager`](./mud/utils/asset_manager.hpp) class,  while the [`Asset<T>`](./mud/utils/asset.hpp) class provides an interface to allocate, free, load, and save assets to the OS's file system. Assets can later be retrieved from the Asset Manager via their original import file path or the UUID which was created when the asset was initially imported. [`AssetObject`](./mud/utils/asset_object.hpp)s are expected to provide an [`ISerialization`](./mud/utils/i_serializable.hpp) interface - this enables the engine to save and load assets in their native format, allowing us to load assets much faster than if we were to import them every time they are required by the program.

- <b>Graphics API abstraction</b>: The engine provides a graphics-API-agnostic interface for rendering. This will allow for future support of other graphics APIs without the need to change users' rendering code. 

- <b>Shader cross-compilation and reflection</b>: By making use of Google's [shaderc](https://github.com/google/shaderc) library and Khronos Group's [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) library, we can take away much of the manual work required by the developer to use their custom shaders in the Mud engine rendering pipelines. Shaders written in `GLSL`, `HLSL`, and `MTL` can all be compiled to SPIR-V modules, which can then be reflected upon to discern the inputs, outputs, uniforms, etc.  
_See: [shader_module_base.hpp](./mud/graphics/interface/shader_module_base.hpp), [vulkan_shader_module.hpp](./mud/graphics/backend/vulkan/vulkan_shader_module.hpp), [spirv.hpp](.mud//graphics/backend/spirv/spirv.hpp)_

- <b>Forward rendering, Deferred rendering (in progress), Sprite batching renderer, Text renderer</b>  
_See: [forward_renderer_base.hpp](./mud/graphics/interface/forward_renderer_base.hpp), [vulkan_forward_renderer.hpp](./mud/graphics/backend/vulkan/vulkan_forward_renderer.hpp), [sprite_batch_renderer_base.hpp](./mud/graphics/interface/sprite_batch_renderer_base.hpp), [vulkan_sprite_batch_renderer.hpp](./mud/graphics/backend/vulkan/vulkan_sprite_batch_renderer.hpp), [text_renderer_base.hpp](./mud/graphics/interface/text_renderer_base.hpp), [vulkan_text_renderer.hpp](./mud/graphics/backend/vulkan/vulkan_text_renderer.hpp)_

- <b>Scene representation, manipulation</b>  
_See: [scene.hpp](./mud/scene.hpp)_

- <b>In-engine CLI</b>: The Mud engine has a simple command console interface which allows the user to reigster commands and execute them by providing the command string populated with the appropriate arguments. This can be done in code or via a CLI at runtime - toggled with the <code>&#96;</code> (Backtick) key.  
_See: [console.hpp](./mud/utils/console.hpp)_
```cpp
using namespace mud;

console::Command * logCommand = new console::Command(
	// The name/id of the command. We'll use this when executing the command
	"log",
	// Helpful description of what the command does
	"Logs a string",
	// List of the parameters this command takes. Type; name; helpful description
	{
		ParameterInfo{ParameterType::String, "log message", "The string to log"}
	},
	// The command function must always have this signature. We can safely assume that 'arguments'
	// will contain the expected number of arguments, each with the expected type.
	[](const std::vector<Argument *> & arguments) -> std::string
	{
            log(LogLevel::Info, arguments[0]->getValue<std::string>() + '\n');
            return "";
        });

console::registerCommand(logCommand);

...

console::execute("log \"Hello, World!\"");
```

- <b>Math suite</b>  
_See: [mud/math/](./mud/math/)_

- <b>Events</b>  
_See: [event.hpp](./mud/platform/event.hpp)_

- <b>Logging</b>: A simple logging system with several levels of log severity, log categories, and text file output. Terminal colours are also supported (uses ANSI escape codes).
_See: [logger.hpp](./mud/utils/logger.hpp)_
```cpp
// Prints a message at the default LogLevel with no category
mud::log("Hello World, I'm Mud!");
// Prints a message at the specified LogLevel with no category
mud::log(mud::LogLevel::Warning, "I'm warning you...");
// Logs a message at the specified LogLevel with the specified category. Using the fmt library to
// safely format our string with variables
mud::log(mud::LogLevel::Error, fmt::format("An error has occured! Message: {0}", errorMessage), "Some Category");
// Logs a message at the specified LogLevel with the specified category, using a specified color to use
// on the terminal rather than the default color for that LogLevel
mud::log(mud::LogLevel::Trace, "Hello World, I'm Mud!", "Test", mud::LogColor::BrightCyan);  
```

#

![alt text](screenshot_2023-07-31.png?raw=true)

_Mud Engine rendering the [Sponza PBR (Intel)](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html) scene using (not very good) PBR shading._

#

## Dependencies

| Library | Usage |
| ------- | ----- |
| [Assimp](https://assimp.org/) | 3D resource importing |
| [fmt](https://github.com/fmtlib/fmt) | Fast and safe string formatting |
| [FreeType 2](https://freetype.org/) | Font importing, glyph rasterizing |
| [GLFW3](https://www.glfw.org/) | Cross-platform windows, input, events |
| [shaderc](https://github.com/google/shaderc) | Compiling shader source to SPIR-V |
| [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) | Cross compiling of shaders |
| [stb_image.h](https://github.com/nothings/stb) | Image importing |
| [Vulkan](https://vkguide.dev/docs/introduction/vulkan_overview/) | Cross platform graphics API |
| [zlib](https://www.zlib.net/) | Data compression - currently only needed by other dependencies |

## Resources

I'm currently making use of various assets for testing. Here's some credits and links to the resources in case you want to download them to be used with the project.

- [Lena](https://links.uwaterloo.ca/Repository.html) - Original  [download link](https://sipi.usc.edu/database/database.php?volume=misc&image=12#top), for historical purposes.
- [Sibenik Cathedral](https://casual-effects.com/g3d/data10/index.html#mesh32) - Model by Marko Dabrovic. Textures by Morgan McGuire. Provided by Morgan McGuire.
- [Sponza (Crytek)](https://casual-effects.com/g3d/data10/index.html#mesh8) - Model by Frank Meinl. Additional textures by Morgan McGuire. Provided by Morgan McGuire.
- [Sponza (Dabrovic)](https://casual-effects.com/g3d/data10/index.html#mesh10) - Model and textures by Marko Dabrovic. Model conversion by Kenzie Lamar. Textures by Morgan McGuire. Provided by Morgan McGuire.
- [Sponza (PBR)](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html) - Intel.
- [Teapot](https://graphics.stanford.edu/courses/cs148-10-summer/as3/code/as3/teapot.obj) - Stanford Computer Graphics Laboratory.
- [Viking Room](https://sketchfab.com/3d-models/viking-room-a49f1b8e4f5c4ecf9e1fe7d81915ad38) - [nigelgoh](https://sketchfab.com/nigelgoh) (Sketchfab)
