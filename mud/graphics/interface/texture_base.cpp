#include "texture_base.hpp"

#include "utils/logger.hpp"

namespace mud
{
	TextureBase::TextureBase()
		: m_data(nullptr), m_width(0), m_height(0), m_channels(0), m_sizeBytes(0)
	{ }

	TextureBase::~TextureBase()
	{
		free(m_data);
	}
	
	const uint8_t * TextureBase::getData() const
	{
		return m_data;
	}

	uint32_t TextureBase::getWidth() const
	{
		return m_width;
	}
	
	uint32_t TextureBase::getHeight() const
	{
		return m_height;
	}

	uint32_t TextureBase::getChannels() const
	{
		return m_channels;
	}

	ImageFormat TextureBase::getImageFormat() const
	{
		return m_imageFormat;
	}

	uint32_t TextureBase::getSizeBytes() const
	{
		return m_sizeBytes;
	}

	void TextureBase::setData(const uint8_t * data, uint32_t width, uint32_t height, uint32_t channels)
	{
		if (data == nullptr)
		{
			log(LogLevel::Error, "Failed to set texture data: passed null data pointer\n", "Texture");
			return;
		}

		const size_t newSize = width * height * channels;

		if (newSize == 0)
		{
			log(LogLevel::Error, "Failed to set texture data: passed invalid size or number of channels\n", "Texture");
			return;
		}

		if (newSize != m_sizeBytes)
		{
			free(m_data);
			m_sizeBytes = newSize;
			m_data = reinterpret_cast<uint8_t*>(malloc(m_sizeBytes));
			
			if (m_data == nullptr)
			{
				log(LogLevel::Error, fmt::format("Failed to set texture data: Failed to allocate memory ({0} bytes)\n", m_sizeBytes), "Texture");
				return;
			}
		}

		memcpy(m_data, data, m_sizeBytes);

		m_width = width;
		m_height = height;
		m_channels = channels;

		switch (m_channels)
		{
			case 1: m_imageFormat = ImageFormat::R8_SRGB; break;
			case 2: m_imageFormat = ImageFormat::R8G8_SRGB; break;
			case 3: m_imageFormat = ImageFormat::R8G8B8_SRGB; break;
			default: m_imageFormat = ImageFormat::R8G8B8A8_SRGB; break;
		}

		onSetData();
	}

	bool TextureBase::deserialize(std::ifstream & file)
	{
		serialization_helpers::deserialize(file, m_width);
		serialization_helpers::deserialize(file, m_height);
		serialization_helpers::deserialize(file, m_channels);
		serialization_helpers::deserialize(file, m_imageFormat);
		m_sizeBytes = m_width * m_height * m_channels;

		m_data = reinterpret_cast<uint8_t *>(malloc(m_sizeBytes));
		serialization_helpers::deserialize(file, m_data, m_sizeBytes);

		onSetData();

		return true;
	}

	bool TextureBase::serialize(std::ofstream & file) const
	{
		serialization_helpers::serialize(file, m_width);
		serialization_helpers::serialize(file, m_height);
		serialization_helpers::serialize(file, m_channels);
		serialization_helpers::serialize(file, m_imageFormat);
		serialization_helpers::serialize(file, m_data, m_sizeBytes);

		return true;
	}

	void TextureBase::freeData()
	{
		m_width = m_height = m_channels = m_sizeBytes = 0;

		free(m_data);
		m_data = nullptr;
	}
}