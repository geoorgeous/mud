#include "asset.hpp"

#include <filesystem>

#include "asset_manager.hpp"
#include "asset_object.hpp"
#include "asset_importer.hpp"
#include "logger.hpp"

namespace mud
{
	const std::string AssetBase::expectedHeaderContent = "mud_asset_file";

	AssetBase::AssetBase()
		: m_object(nullptr)
	{
		m_uuid.generate();
	}

	AssetBase::AssetBase(const UUID & uuid)
		: m_object(nullptr), m_uuid(uuid)
	{ }

	AssetBase::~AssetBase()
	{
		unload();
	}

	void AssetBase::unload()
	{
		delete m_object;
		m_object = nullptr;
	}

	const UUID & AssetBase::getUuid() const
	{
		return m_uuid;
	}

	const std::string & AssetBase::getFilepath() const
	{
		return m_filepath;
	}

	const std::string & AssetBase::getImportFilepath() const
	{
		return m_importFilepath;
	}

	void AssetBase::setImportFilepath(const std::string & filepath)
	{
		m_importFilepath = filepath;
	}

	bool AssetBase::allocateObject()
	{
		return allocateObjectInternal();
	}

	void AssetBase::move(const std::string & filepath)
	{
		if (m_filepath == filepath)
			return;

		if (!m_filepath.empty())
			std::filesystem::rename(m_filepath, filepath);

		m_filepath = filepath;
	}

	bool AssetBase::save()
	{
		if (m_object == nullptr)
		{
			log(LogLevel::Error, "Failed to save asset: No asset object allocated to save\n", "Asset");
			return false;
		}

		if (m_filepath.empty())
			m_filepath = AssetManager::createAssetFilepath(m_importFilepath.empty() ? m_uuid.getString() : std::filesystem::path(m_importFilepath).filename().string());

		std::filesystem::create_directory(AssetManager::assetDirectory);
		std::filesystem::create_directories(std::filesystem::path(m_filepath).parent_path());

		std::ofstream file(m_filepath, std::ios::binary);
		if (!file)
		{
			log(LogLevel::Error, fmt::format("Failed to save asset '{0}': Could not open file to write\n", m_filepath), "Asset");
			return false;
		}

		serialization_helpers::serialize(file, expectedHeaderContent);
		serialization_helpers::serialize(file, m_object->getType());
		m_uuid.serialize(file);
		serialization_helpers::serialize(file, m_importFilepath);
		return m_object->serialize(file);
	}

	bool AssetBase::save(const std::string & filepath)
	{
		move(filepath);
		return save();
	}

	bool AssetBase::load(bool loadAssetObject)
	{
		std::ifstream file;
		if (!tryOpenIFile(file))
			return false;

		AssetMetaData metaData;
		if (!deserializeMetaData(file, m_filepath, metaData))
			return false;

		m_uuid = metaData.uuid;
		m_importFilepath = metaData.importFilepath;

		if (loadAssetObject)
		{
			if (!allocateObjectInternal())
			{
				log(LogLevel::Error, fmt::format("Failed to load asset '{0}': Failed to allocate asset object\n", m_filepath), "Asset");
				return false;
			}

			return m_object->deserialize(file);
		}
		
		return true;
	}

	bool AssetBase::load(const std::string & filepath, bool loadAssetObject)
	{
		move(filepath);
		return load(loadAssetObject);
	}

	bool AssetBase::isObjectLoaded() const
	{
		return m_object != nullptr;
	}

	bool AssetBase::deleteLocalFile()
	{
		if (!m_filepath.empty() && !std::filesystem::remove(m_filepath))
		{
			log(LogLevel::Warning, fmt::format("Failed to delete asset file '{0}': File system error\n", m_filepath), "Asset");
			return false;
		}
		m_filepath.clear();
		return true;
	}

	bool AssetBase::serializeReference(std::ofstream & file) const
	{
		return m_uuid.serialize(file);
	}

	bool AssetBase::deserializeMetaDataFromFile(const std::string & filepath, AssetMetaData & metaData)
	{
		std::ifstream file(filepath, std::ios::binary);
		if (!file)
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': could not open file to read\n", filepath), "Asset");
			return false;
		}

		return deserializeMetaData(file, filepath, metaData);
	}
	
	bool AssetBase::deserializeMetaData(std::ifstream & file, const std::string & filepath, AssetMetaData & metaData)
	{
		if (!serialization_helpers::deserialize(file, metaData.header))
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': failed to read file header\n", filepath), "Asset");
			return false;
		}

		if (metaData.header != expectedHeaderContent)
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': unexpected or corrupt header content ({1})\n", filepath, metaData.header), "Asset");
			return false;
		}

		if (!serialization_helpers::deserialize(file, metaData.type))
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': failed to read file type\n", filepath), "Asset");
			return false;
		}

		if (!metaData.uuid.deserialize(file))
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': Failed to read asset UUID\n", filepath), "Asset");
			return false;
		}

		if (!metaData.uuid.isValid())
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': UUID is invalid: '{1}'\n", filepath, metaData.uuid.getString()), "Asset");
			return false;
		}

		if (!serialization_helpers::deserialize(file, metaData.importFilepath))
		{
			log(LogLevel::Error, fmt::format("Failed to deserialize asset meta data from file '{0}': Failed to deserialize import filepath\n", metaData.importFilepath), "Asset");
			return false;
		}

		return true;
	}
	
	bool AssetBase::deserializeObject() const
	{
		std::ifstream file;
		if (!tryOpenIFile(file))
			return false;

		AssetMetaData metaData;
		if (!deserializeMetaData(file, m_filepath, metaData))
			return false;

		if (!allocateObjectInternal())
		{
			log(LogLevel::Error, fmt::format("Failed to load asset '{0}': Failed to allocate asset object\n", m_filepath), "Asset");
			return false;
		}

		return m_object->deserialize(file);
	}

	bool AssetBase::tryOpenIFile(std::ifstream & file) const
	{
		if (m_filepath.empty())
		{
			log(LogLevel::Error, fmt::format("Failed to open asset file to read: No filepath. UUID: {0}, Import filepath: {1}\n", m_uuid.getString(), m_importFilepath), "Asset");
			return false;
		}
		
		file.open(m_filepath, std::ios::binary);
		if (!file)
		{
			log(LogLevel::Error, fmt::format("Failed to open asset file to read: '{0}': {1}\n", m_filepath, std::string(std::strerror(errno))), "Asset");
			return false;
		}

		return true;
	}
}