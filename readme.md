#

<p align="right">
  <img src="./mud_logo.png?raw=true" alt="Sublime's custom image"/>
</p>

# Mud Engine

This repo does not currently include the dependencies used for the project, or many of the resources that might be references in the source. See below for credits and download links.

## Features
- <b>Cross platform</b>: The engine is designed to be platform agnostic. This is achieved by using the [GLTF](https://www.glfw.org/) library to handle cross platform windowing and input, and by writing the rendering backend in [Vulkan](https://vkguide.dev/docs/introduction/vulkan_overview/) first and foremost.

- <b>Asset management</b>
- <b>Graphics API abstraction</b>
- <b>Shader cross-compilation and reflection</b>
- <b>Forward rendering</b>
- <b>Deferred rendering (in progress)</b>
- <b>Sprite batching renderer</b>
- <b>Text renderer</b>
- <b>Scene representation, manipulation</b>
- <b>In-engine CLI</b>
- <b>Math suite</b>
- <b>Events</b>
- <b>Logging</b>

#

![alt text](screenshot_2023-07-31.png?raw=true)

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
