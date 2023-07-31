#ifndef SHADER_MODULE_BASE_HPP
#define SHADER_MODULE_BASE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "graphics/e_shader_type.hpp"

namespace mud
{
	struct ShaderModulePushConstantBlockDetails
	{
		size_t offset;
		size_t size;
		std::unordered_map<std::string, size_t> memberOffsets;
	};

	class ShaderModulePushConstantBlock
	{
	public:

		ShaderModulePushConstantBlock(const ShaderModulePushConstantBlockDetails & details, ShaderType shaderType);

		ShaderModulePushConstantBlock(const ShaderModulePushConstantBlock&) = delete;
		ShaderModulePushConstantBlock& operator=(const ShaderModulePushConstantBlock&) = delete;
		ShaderModulePushConstantBlock(ShaderModulePushConstantBlock&&) = delete;
		ShaderModulePushConstantBlock& operator=(ShaderModulePushConstantBlock&&) = delete;

		~ShaderModulePushConstantBlock();

		ShaderType getShaderType() const;

		size_t getOffset() const;

		const void * getData() const;

		size_t getSize() const;

		template<typename T>
		T * getMember(const std::string & memberId)
		{
			auto iter = m_members.find(memberId);
			if (iter == m_members.end())
				return nullptr;
			return reinterpret_cast<T *>(iter->second);
		}

	private:

		ShaderType m_shaderType;
		size_t m_offset;
		size_t m_size;
		uint8_t * m_data;
		std::unordered_map<std::string, uint8_t *> m_members;
	};

	class ShaderModuleBase
	{
	public:

		ShaderModuleBase(ShaderType type);

		ShaderType getType() const;

		const std::string & getSource() const;

		void setSource(const std::string & source);

		virtual bool compileSource();

		const std::vector<uint32_t> & getSpirvByteCode() const;

		const std::unordered_map<std::string, ShaderModulePushConstantBlockDetails> & getPushConstantBlocksDetails() const;

	protected:

		ShaderType m_type;
		std::string m_source;
		std::vector<uint32_t> m_spirvByteCode;
		std::unordered_map<std::string, ShaderModulePushConstantBlockDetails> m_pushConstantBlocksDetails;
	};
}

#endif