#include <string>
#include <vector>

#include "graphics/e_shader_type.hpp"

#include "spirv_reflect.h"

namespace mud::graphics_backend::spirv
{
    // Returns GLSL shader source text after preprocessing.
    std::string preprocessSource(const std::string & sourceName, ShaderType type, const std::string & source);

    // Compiles a shader to SPIR-V assembly. Returns the assembly text as a string.
    std::string compileSourceToSPIRVAssembly(const std::string & sourceName, ShaderType type, const std::string & source, bool optimize = false);

    // Compiles a shader to a SPIR-V binary. Returns the binary as a vector of 32-bit words.
    std::vector<uint32_t> compileSourceToSPIRV(const std::string & sourceName, ShaderType type, const std::string & source, bool optimize = false);

    std::string spirvToGLSL(const std::vector<uint32_t> & spirv);

	std::string toString(SpvBuiltIn spvBuiltIn);
    
	std::string toString(SpvReflectTypeDescription spvReflectTypeDescription);
    
	std::string toString(SpvReflectFormat spvReflectFormat);
    
	std::string toString(SpvReflectDescriptorType spvReflectDescriptorType);

    size_t sizeOfType(SpvReflectTypeDescription spvReflectTypeDescription);
}