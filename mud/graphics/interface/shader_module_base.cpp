#include "shader_module_base.hpp"

#include <sstream>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "graphics/backend/spirv/spirv.hpp"
#include "utils/logger.hpp"

namespace mud
{
	void printSprivVariable(const SpvReflectInterfaceVariable & var, std::stringstream & ss)
	{
		ss << "\t\t> name: ";
		if ((var.name != nullptr) && (strlen(var.name) > 0))
			ss << var.name;
		if ((var.type_description->type_name != nullptr) && (strlen(var.type_description->type_name) > 0))
			ss << " (" << var.type_description->type_name << ")";
		ss << "\n";

		ss << "\t\tlocation: ";
		if (var.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
			ss << graphics_backend::spirv::toString(var.built_in) << " (built-in)";
		else
			ss << var.location;
		ss << "\n";

		if (var.semantic != nullptr && (strlen(var.semantic) > 0))
			ss << "semantic: " << var.semantic << "\n";

		ss << "\t\ttype: " << graphics_backend::spirv::toString(*var.type_description) << "\n";

		ss << "\t\tformat: " << graphics_backend::spirv::toString(var.format) << "\n";

		ss << "\t\tsize: " << graphics_backend::spirv::sizeOfType(*var.type_description) << "\n";

		ss << "\t\tqualifier: ";
		if (var.decoration_flags & SPV_REFLECT_DECORATION_FLAT)
			ss << "flat";
		else if (var.decoration_flags & SPV_REFLECT_DECORATION_NOPERSPECTIVE)
			ss << "noperspective";
		ss << "\n";
	}

	void printSprivDescriptorBinding(const SpvReflectDescriptorBinding & binding, std::stringstream & ss)
	{
		ss << "\t\t\tbinding: " << binding.binding << "\n";
		ss << "\t\t\ttype: " << graphics_backend::spirv::toString(binding.descriptor_type) << "\n";

		// array
		if (binding.array.dims_count > 0)
		{
			ss << "\t\t\tarray: ";
			for (uint32_t dim_index = 0; dim_index < binding.array.dims_count; ++dim_index)
				ss << "[" << binding.array.dims[dim_index] << "]";
			ss << "\n";
		}

		// counter
		if (binding.uav_counter_binding != nullptr)
		{
			ss << "\t\t\tcounter: (";
			ss << "set=" << binding.uav_counter_binding->set << ", ";
			ss << "binding=" << binding.uav_counter_binding->binding << ", ";
			ss << "name=" << binding.uav_counter_binding->name << ");\n";
		}

		ss << "\t\t\tname: ";
		if ((binding.name != nullptr) && (strlen(binding.name) > 0))
			ss << binding.name;
		if ((binding.type_description->type_name != nullptr) && (strlen(binding.type_description->type_name) > 0))
			ss << " (" << binding.type_description->type_name << ")";
		ss << "\n";
	}

	void printSprivDescriptorSet(const SpvReflectDescriptorSet & set, std::stringstream & ss)
	{
		ss << "\t\tset: " << set.set << "\n";
		ss << "\t\tbinding count: " << set.binding_count << "\n";
		for (uint32_t i = 0; i < set.binding_count; ++i)
		{
			ss << "\t\t\t> " << i << ":" << "\n";
			printSprivDescriptorBinding(*set.bindings[i], ss);
		}
	}

	void printSpirvBlockVariable(const SpvReflectBlockVariable & block, std::stringstream & ss)
	{
		ss << "\t\tname: " << block.name << "\n";
		ss << "\t\tsize: " << block.size << " (padded: " << block.padded_size << ")\n";
		ss << "\t\tabsolute offset:" << block.absolute_offset << " (" << block.offset << ")\n";
		ss << "\t\tmembers (" << block.member_count << ")\n";
		for (uint32_t i = 0; i < block.member_count; ++i)
		{
			const SpvReflectBlockVariable & member = block.members[i];
			ss << "\t\t\t[" << i << "]name: " << member.name << "\n";
			ss << "\t\t\tsize: " << member.size << " (padded: " << member.padded_size << ")\n";
			ss << "\t\t\tabsolute offset:" << member.absolute_offset << " (" << member.offset << ")\n";
		}
	}

	void printSprivModuleInfo(const SpvReflectShaderModule & module, std::stringstream & ss)
	{
		ss << "\tentry point: " << module.entry_point_name << "\n";
		ss << "\tsource lang: " << spvReflectSourceLanguage(module.source_language) << "\n";
		ss << "\tsource lang ver: " << module.source_language_version << "\n";
		if (module.source_language == SpvSourceLanguageHLSL)
		{
			ss << "stage: ";
			switch (module.shader_stage)
			{
			default:
				break;
			case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
				ss << "VS";
				break;
			case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				ss << "HS";
				break;
			case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				ss << "DS";
				break;
			case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
				ss << "GS";
				break;
			case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
				ss << "PS";
				break;
			case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
				ss << "CS";
				break;
			}
		}
	}

	ShaderModulePushConstantBlock::ShaderModulePushConstantBlock(const ShaderModulePushConstantBlockDetails & details, ShaderType shaderType)
		: m_shaderType(shaderType), m_offset(details.offset), m_size(details.size)
	{
		m_data = new uint8_t[m_size];

		for (const auto & pair : details.memberOffsets)
			m_members[pair.first] = m_data + pair.second;
	}

	ShaderModulePushConstantBlock::~ShaderModulePushConstantBlock()
	{
		delete[] m_data;
	}

	ShaderType ShaderModulePushConstantBlock::getShaderType() const
	{
		return m_shaderType;
	}

	size_t ShaderModulePushConstantBlock::getOffset() const
	{
		return m_offset;
	}

	const void * ShaderModulePushConstantBlock::getData() const
	{
		return m_data;
	}

	size_t ShaderModulePushConstantBlock::getSize() const
	{
		return m_size;
	}

	ShaderModuleBase::ShaderModuleBase(ShaderType type)
		: m_type(type)
	{}

	ShaderType ShaderModuleBase::getType() const
	{
		return m_type;
	}

	const std::string & ShaderModuleBase::getSource() const
	{
		return m_source;
	}

	void ShaderModuleBase::setSource(const std::string & source)
	{
		m_source = source;
		m_spirvByteCode.clear();
	}

	bool ShaderModuleBase::compileSource()
	{
		shaderc::CompileOptions compileOptions;
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
		compileOptions.SetGenerateDebugInfo();

		shaderc_shader_kind kind;

		switch (m_type)
		{
		case ShaderType::Vertex:
			kind = shaderc_glsl_vertex_shader;
			break;
		case ShaderType::Fragment:
			kind = shaderc_glsl_fragment_shader;
			break;
		}

		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(m_source, kind, "mud shader", compileOptions);

		if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			log(LogLevel::Error, fmt::format("Failed to compile shader to SPIR-V: {0}", compilationResult.GetErrorMessage()), "SPIR-V");
			return false;
		}

		m_spirvByteCode = { compilationResult.cbegin(), compilationResult.cend() };

		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(m_spirvByteCode.size() * sizeof(uint32_t), static_cast<void *>(m_spirvByteCode.data()), &module);

		if (result != SPV_REFLECT_RESULT_SUCCESS)
			return false;

		uint32_t count = 0;

		//std::stringstream ss;
		//ss << "Shader Reflection\n";

		//printSprivModuleInfo(module, ss);

		// result = spvReflectEnumerateInputVariables(&module, &count, NULL);
		// if (count > 0)
		// {
		// 	ss << "\tInput variables (" << count << ")\n";
		// 	std::vector<SpvReflectInterfaceVariable *> inputVars(count);
		// 	result = spvReflectEnumerateInputVariables(&module, &count, inputVars.data());
		// 	for (SpvReflectInterfaceVariable * var : inputVars)
		// 		printSprivVariable(*var, ss);
		// }

		// result = spvReflectEnumerateOutputVariables(&module, &count, NULL);
		// if (count > 0)
		// {
		// 	ss << "\tOutput variables (" << count << ")\n";
		// 	std::vector<SpvReflectInterfaceVariable *> outputVars(count);
		// 	result = spvReflectEnumerateOutputVariables(&module, &count, outputVars.data());
		// 	for (SpvReflectInterfaceVariable * var : outputVars)
		// 		printSprivVariable(*var, ss);
		// }

		// result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		// if (count > 0)
		// {
		// 	ss << "\tDescriptor sets (" << count << ")\n";
		// 	std::vector<SpvReflectDescriptorSet *> descriptorSets(count);
		// 	result = spvReflectEnumerateDescriptorSets(&module, &count, descriptorSets.data());
		// 	for (const SpvReflectDescriptorSet * set : descriptorSets)
		// 	{
		// 		const SpvReflectDescriptorSet & descriptorSet = *set;
		// 		printSprivDescriptorSet(descriptorSet, ss);
		// 	}
		// }

		result = spvReflectEnumeratePushConstantBlocks(&module, &count, NULL);
		if (count > 0)
		{
			//ss << "\tPush constant blocks (" << count << ")\n";
			std::vector<SpvReflectBlockVariable *> pushConstantBlocks(count);
			result = spvReflectEnumeratePushConstantBlocks(&module, &count, pushConstantBlocks.data());
			for (const SpvReflectBlockVariable * block : pushConstantBlocks)
			{
				const SpvReflectBlockVariable & pushConstantBlock = *block;
				//printSpirvBlockVariable(pushConstantBlock, ss);

				ShaderModulePushConstantBlockDetails & details = m_pushConstantBlocksDetails[pushConstantBlock.name];
				details.offset = pushConstantBlock.members[0].absolute_offset;
				details.size = static_cast<size_t>(pushConstantBlock.size - details.offset);

				for (uint32_t i = 0; i < pushConstantBlock.member_count; ++i)
				{
					const SpvReflectBlockVariable & member = pushConstantBlock.members[i];
					details.memberOffsets[member.name] = member.absolute_offset - details.offset;
				}
			}
		}

		//log(LogLevel::Info, ss.str(), "SPIR-V");

		spvReflectDestroyShaderModule(&module);

		return true;
	}

	const std::vector<uint32_t> & ShaderModuleBase::getSpirvByteCode() const
	{
		return m_spirvByteCode;
	}

	const std::unordered_map<std::string, ShaderModulePushConstantBlockDetails> & ShaderModuleBase::getPushConstantBlocksDetails() const
	{
		return m_pushConstantBlocksDetails;
	}
}