#include "spirv.hpp"

#include <fmt/format.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <sstream>

#include "utils/logger.hpp"

namespace mud::graphics_backend::spirv
{
    std::string preprocessSource(const std::string &sourceName, ShaderType type, const std::string &source)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        // options.AddMacroDefinition("MY_DEFINE", "1");

        shaderc_shader_kind kind;

        switch (type)
        {
            case ShaderType::Vertex: kind = shaderc_glsl_vertex_shader; break;
            case ShaderType::Fragment: kind = shaderc_glsl_fragment_shader; break;
        }

        shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(source, kind, sourceName.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            log (LogLevel::Error, fmt::format("Failed to preprocess shader source: {0}", result.GetErrorMessage()), "SPIR-V");
            return "";
        }

        return {result.cbegin(), result.cend()};
    }

    std::string compileSourceToSPIRVAssembly(const std::string & sourceName, ShaderType type, const std::string & source, bool optimize)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        if (optimize)
        {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        shaderc_shader_kind kind;

        switch (type)
        {
            case ShaderType::Vertex: kind = shaderc_glsl_vertex_shader; break;
            case ShaderType::Fragment: kind = shaderc_glsl_fragment_shader; break;
        }

        shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(source, kind, sourceName.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            log (LogLevel::Error, fmt::format("Failed to compile shader to SPIR-V assembly: {0}", result.GetErrorMessage()), "SPIR-V");
            return "";
        }

        return {result.cbegin(), result.cend()};
    }

    std::vector<uint32_t> compileSourceToSPIRV(const std::string & sourceName, ShaderType type, const std::string & source, bool optimize)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        if (optimize)
        {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        shaderc_shader_kind kind;

        switch (type)
        {
            case ShaderType::Vertex: kind = shaderc_glsl_vertex_shader; break;
            case ShaderType::Fragment: kind = shaderc_glsl_fragment_shader; break;
        }

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, sourceName.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            log (LogLevel::Error, fmt::format("Failed to compile shader to SPIR-V: {0}", module.GetErrorMessage()), "SPIR-V");
            return std::vector<uint32_t>();
        }

        return {module.cbegin(), module.cend()};
    }

    std::string spirvToGLSL(const std::vector<uint32_t> & spirv)
    {
        spirv_cross::CompilerGLSL glsl(spirv);

        spirv_cross::ShaderResources resources = glsl.get_shader_resources();

        spirv_cross::CompilerGLSL::Options options;
	    options.version = 310;
	    options.es = true;
	    glsl.set_common_options(options);

        // Compile to GLSL, ready to give to GL driver.
	    std::string source = glsl.compile();

        return source;
    }

	std::string toString(SpvBuiltIn spvBuiltIn)
    {
		switch (spvBuiltIn)
		{
		case SpvBuiltInPosition:
			return "Position";
		case SpvBuiltInPointSize:
			return "PointSize";
		case SpvBuiltInClipDistance:
			return "ClipDistance";
		case SpvBuiltInCullDistance:
			return "CullDistance";
		case SpvBuiltInVertexId:
			return "VertexId";
		case SpvBuiltInInstanceId:
			return "InstanceId";
		case SpvBuiltInPrimitiveId:
			return "PrimitiveId";
		case SpvBuiltInInvocationId:
			return "InvocationId";
		case SpvBuiltInLayer:
			return "Layer";
		case SpvBuiltInViewportIndex:
			return "ViewportIndex";
		case SpvBuiltInTessLevelOuter:
			return "TessLevelOuter";
		case SpvBuiltInTessLevelInner:
			return "TessLevelInner";
		case SpvBuiltInTessCoord:
			return "TessCoord";
		case SpvBuiltInPatchVertices:
			return "PatchVertices";
		case SpvBuiltInFragCoord:
			return "FragCoord";
		case SpvBuiltInPointCoord:
			return "PointCoord";
		case SpvBuiltInFrontFacing:
			return "FrontFacing";
		case SpvBuiltInSampleId:
			return "SampleId";
		case SpvBuiltInSamplePosition:
			return "SamplePosition";
		case SpvBuiltInSampleMask:
			return "SampleMask";
		case SpvBuiltInFragDepth:
			return "FragDepth";
		case SpvBuiltInHelperInvocation:
			return "HelperInvocation";
		case SpvBuiltInNumWorkgroups:
			return "NumWorkgroups";
		case SpvBuiltInWorkgroupSize:
			return "WorkgroupSize";
		case SpvBuiltInWorkgroupId:
			return "WorkgroupId";
		case SpvBuiltInLocalInvocationId:
			return "LocalInvocationId";
		case SpvBuiltInGlobalInvocationId:
			return "GlobalInvocationId";
		case SpvBuiltInLocalInvocationIndex:
			return "LocalInvocationIndex";
		case SpvBuiltInWorkDim:
			return "WorkDim";
		case SpvBuiltInGlobalSize:
			return "GlobalSize";
		case SpvBuiltInEnqueuedWorkgroupSize:
			return "EnqueuedWorkgroupSize";
		case SpvBuiltInGlobalOffset:
			return "GlobalOffset";
		case SpvBuiltInGlobalLinearId:
			return "GlobalLinearId";
		case SpvBuiltInSubgroupSize:
			return "SubgroupSize";
		case SpvBuiltInSubgroupMaxSize:
			return "SubgroupMaxSize";
		case SpvBuiltInNumSubgroups:
			return "NumSubgroups";
		case SpvBuiltInNumEnqueuedSubgroups:
			return "NumEnqueuedSubgroups";
		case SpvBuiltInSubgroupId:
			return "SubgroupId";
		case SpvBuiltInSubgroupLocalInvocationId:
			return "SubgroupLocalInvocationId";
		case SpvBuiltInVertexIndex:
			return "VertexIndex";
		case SpvBuiltInInstanceIndex:
			return "InstanceIndex";
		case SpvBuiltInSubgroupEqMaskKHR:
			return "SubgroupEqMaskKHR";
		case SpvBuiltInSubgroupGeMaskKHR:
			return "SubgroupGeMaskKHR";
		case SpvBuiltInSubgroupGtMaskKHR:
			return "SubgroupGtMaskKHR";
		case SpvBuiltInSubgroupLeMaskKHR:
			return "SubgroupLeMaskKHR";
		case SpvBuiltInSubgroupLtMaskKHR:
			return "SubgroupLtMaskKHR";
		case SpvBuiltInBaseVertex:
			return "BaseVertex";
		case SpvBuiltInBaseInstance:
			return "BaseInstance";
		case SpvBuiltInDrawIndex:
			return "DrawIndex";
		case SpvBuiltInDeviceIndex:
			return "DeviceIndex";
		case SpvBuiltInViewIndex:
			return "ViewIndex";
		case SpvBuiltInBaryCoordNoPerspAMD:
			return "BaryCoordNoPerspAMD";
		case SpvBuiltInBaryCoordNoPerspCentroidAMD:
			return "BaryCoordNoPerspCentroidAMD";
		case SpvBuiltInBaryCoordNoPerspSampleAMD:
			return "BaryCoordNoPerspSampleAMD";
		case SpvBuiltInBaryCoordSmoothAMD:
			return "BaryCoordSmoothAMD";
		case SpvBuiltInBaryCoordSmoothCentroidAMD:
			return "BaryCoordSmoothCentroidAMD";
		case SpvBuiltInBaryCoordSmoothSampleAMD:
			return "BaryCoordSmoothSampleAMD";
		case SpvBuiltInBaryCoordPullModelAMD:
			return "BaryCoordPullModelAMD";
		case SpvBuiltInFragStencilRefEXT:
			return "FragStencilRefEXT";
		case SpvBuiltInViewportMaskNV:
			return "ViewportMaskNV";
		case SpvBuiltInSecondaryPositionNV:
			return "SecondaryPositionNV";
		case SpvBuiltInSecondaryViewportMaskNV:
			return "SecondaryViewportMaskNV";
		case SpvBuiltInPositionPerViewNV:
			return "PositionPerViewNV";
		case SpvBuiltInViewportMaskPerViewNV:
			return "ViewportMaskPerViewNV";
		case SpvBuiltInLaunchIdKHR:
			return "InLaunchIdKHR";
		case SpvBuiltInLaunchSizeKHR:
			return "InLaunchSizeKHR";
		case SpvBuiltInWorldRayOriginKHR:
			return "InWorldRayOriginKHR";
		case SpvBuiltInWorldRayDirectionKHR:
			return "InWorldRayDirectionKHR";
		case SpvBuiltInObjectRayOriginKHR:
			return "InObjectRayOriginKHR";
		case SpvBuiltInObjectRayDirectionKHR:
			return "InObjectRayDirectionKHR";
		case SpvBuiltInRayTminKHR:
			return "InRayTminKHR";
		case SpvBuiltInRayTmaxKHR:
			return "InRayTmaxKHR";
		case SpvBuiltInInstanceCustomIndexKHR:
			return "InInstanceCustomIndexKHR";
		case SpvBuiltInObjectToWorldKHR:
			return "InObjectToWorldKHR";
		case SpvBuiltInWorldToObjectKHR:
			return "InWorldToObjectKHR";
		case SpvBuiltInHitTNV:
			return "InHitTNV";
		case SpvBuiltInHitKindKHR:
			return "InHitKindKHR";
		case SpvBuiltInIncomingRayFlagsKHR:
			return "InIncomingRayFlagsKHR";
		case SpvBuiltInRayGeometryIndexKHR:
			return "InRayGeometryIndexKHR";

		case SpvBuiltInMax:
		default:
			break;
		}
		// unhandled SpvBuiltIn enum value
		std::stringstream ss;
		ss << "??? (" << spvBuiltIn << ")";
		return ss.str();
	}
    
	std::string toString(SpvReflectTypeDescription spvReflectTypeDescription)
    {
		switch (spvReflectTypeDescription.op)
		{
		case SpvOpTypeVector:
			switch (spvReflectTypeDescription.traits.numeric.scalar.width)
			{
			case 32:
				switch (spvReflectTypeDescription.traits.numeric.vector.component_count)
				{
				case 2:
					return "vec2";
				case 3:
					return "vec3";
				case 4:
					return "vec4";
				}
				break;

			case 64:
				switch (spvReflectTypeDescription.traits.numeric.vector.component_count)
				{
				case 2:
					return "dvec2";
				case 3:
					return "dvec3";
				case 4:
					return "dvec4";
				}
				break;
			}
			break;

		default:
			break;
		}

		switch (spvReflectTypeDescription.op)
		{
		case SpvOpTypeVoid:
			return "void";
			break;

		case SpvOpTypeBool:
			return "bool";
			break;

		case SpvOpTypeInt:
			if (spvReflectTypeDescription.traits.numeric.scalar.signedness)
				return "int";
			else
				return "uint";

		case SpvOpTypeFloat:
			switch (spvReflectTypeDescription.traits.numeric.scalar.width)
			{
			case 32:
				return "float";
			case 64:
				return "double";
			default:
				break;
			}
			break;

		case SpvOpTypeStruct:
			return "struct";

		default:
			break;
		}

		return "???";
	}
    
	std::string toString(SpvReflectFormat spvReflectFormat)
    {
		switch (spvReflectFormat)
		{
		case SPV_REFLECT_FORMAT_UNDEFINED:
			return "VK_FORMAT_UNDEFINED";
		case SPV_REFLECT_FORMAT_R32_UINT:
			return "VK_FORMAT_R32_UINT";
		case SPV_REFLECT_FORMAT_R32_SINT:
			return "VK_FORMAT_R32_SINT";
		case SPV_REFLECT_FORMAT_R32_SFLOAT:
			return "VK_FORMAT_R32_SFLOAT";
		case SPV_REFLECT_FORMAT_R32G32_UINT:
			return "VK_FORMAT_R32G32_UINT";
		case SPV_REFLECT_FORMAT_R32G32_SINT:
			return "VK_FORMAT_R32G32_SINT";
		case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
			return "VK_FORMAT_R32G32_SFLOAT";
		case SPV_REFLECT_FORMAT_R32G32B32_UINT:
			return "VK_FORMAT_R32G32B32_UINT";
		case SPV_REFLECT_FORMAT_R32G32B32_SINT:
			return "VK_FORMAT_R32G32B32_SINT";
		case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
			return "VK_FORMAT_R32G32B32_SFLOAT";
		case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
			return "VK_FORMAT_R32G32B32A32_UINT";
		case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
			return "VK_FORMAT_R32G32B32A32_SINT";
		case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
			return "VK_FORMAT_R32G32B32A32_SFLOAT";
		case SPV_REFLECT_FORMAT_R64_UINT:
			return "VK_FORMAT_R64_UINT";
		case SPV_REFLECT_FORMAT_R64_SINT:
			return "VK_FORMAT_R64_SINT";
		case SPV_REFLECT_FORMAT_R64_SFLOAT:
			return "VK_FORMAT_R64_SFLOAT";
		case SPV_REFLECT_FORMAT_R64G64_UINT:
			return "VK_FORMAT_R64G64_UINT";
		case SPV_REFLECT_FORMAT_R64G64_SINT:
			return "VK_FORMAT_R64G64_SINT";
		case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
			return "VK_FORMAT_R64G64_SFLOAT";
		case SPV_REFLECT_FORMAT_R64G64B64_UINT:
			return "VK_FORMAT_R64G64B64_UINT";
		case SPV_REFLECT_FORMAT_R64G64B64_SINT:
			return "VK_FORMAT_R64G64B64_SINT";
		case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
			return "VK_FORMAT_R64G64B64_SFLOAT";
		case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
			return "VK_FORMAT_R64G64B64A64_UINT";
		case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
			return "VK_FORMAT_R64G64B64A64_SINT";
		case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
			return "VK_FORMAT_R64G64B64A64_SFLOAT";
		}
		// unhandled SpvReflectFormat enum value
		return "VK_FORMAT_???";
	}
    
	std::string toString(SpvReflectDescriptorType spvReflectDescriptorType)
    {
		switch (spvReflectDescriptorType)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
			return "VK_DESCRIPTOR_TYPE_SAMPLER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
			return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
		}
		// unhandled SpvReflectDescriptorType enum value
		return "VK_DESCRIPTOR_TYPE_???";
	}

    size_t sizeOfType(SpvReflectTypeDescription spvReflectTypeDescription)
    {
		switch (spvReflectTypeDescription.op)
		{
		case SpvOpTypeVector:
            return (spvReflectTypeDescription.traits.numeric.scalar.width / 8) * spvReflectTypeDescription.traits.numeric.vector.component_count;
		case SpvOpTypeBool:
			return sizeof(bool);
		case SpvOpTypeInt:
			return sizeof(int);
		case SpvOpTypeFloat:
            return (spvReflectTypeDescription.traits.numeric.scalar.width / 8);
		case SpvOpTypeStruct:
		{
            size_t total = 0;
            for (size_t idx = 0; idx < spvReflectTypeDescription.member_count; ++idx)
                total += sizeOfType(spvReflectTypeDescription.members[idx]);
            return total;
		}
		default:
			return 0;
		}
	}
}