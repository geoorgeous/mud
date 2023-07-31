#ifndef TEXTURE_BASE_HPP
#define TEXTURE_BASE_HPP

#include <stdint.h>
#include <string>

#include "graphics/image_format.hpp"
#include "math/vector.hpp"
#include "utils/asset_object.hpp"

namespace mud
{
	class TextureBase : public AssetObject<TextureBase>
	{
	public:

		TextureBase();

		~TextureBase();

		const uint8_t * getData() const;

		uint32_t getWidth() const;

		uint32_t getHeight() const;

		uint32_t getChannels() const;

		ImageFormat getImageFormat() const;

		uint32_t getSizeBytes() const;

		void setData(const uint8_t * data, uint32_t width, uint32_t height, uint32_t channels);

		virtual bool deserialize(std::ifstream & file) override;

		virtual bool serialize(std::ofstream & file) const override;

	protected:

		uint8_t * m_data;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_channels;
		ImageFormat m_imageFormat;
		size_t m_sizeBytes;

		virtual void onSetData() = 0;

		virtual void freeData();
	};

	template<>
	inline AssetObject<TextureBase>::AssetObject()
		: AssetObjectBase(AssetObjectType::Texture)
	{ }
}

#endif