#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <filesystem>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "asset_importer.hpp"
#include "asset.hpp"
#include "logger.hpp"

namespace mud
{
	class AssetManager
	{
	public:

		static const std::string assetDirectory;
		static const std::string assetFileExtension;

		static AssetManager & getInstance();

		static std::string createAssetFilepath(const std::string & relativeFilepath);

		template <typename T>
		static const Asset<T> * getDefaultAsset()
		{
			return nullptr;
		}

		AssetManager();

		~AssetManager();

		void makeFilepathUnique(std::string & filepath) const;

		std::string createUniqueAssetFilepath(const std::string & relativeFilepath);

		void importLocalAssets();

		void unloadAssets();

		bool importAssetUnknownType(const std::string & filepath);

		bool importDirectory(const std::string & directory);

		template <typename T>
		Asset<T> * newAsset()
		{
			Asset<T> * newAsset = new Asset<T>();
			m_assets[newAsset->getUuid()] = newAsset;
			return newAsset;
		}

		template <typename T>
		Asset<T> * deserializeAssetReference(std::ifstream & file)
		{
			UUID assetUuid;

			if (!assetUuid.deserialize(file) || !assetUuid.isValid())
			{
				log(LogLevel::Error, fmt::format("Failed to deserialize asset reference: Base UUID: '{0}'\n", assetUuid.getString()), "Asset");
				return nullptr;
			}

			auto iter = m_assets.find(assetUuid);

			if (iter != m_assets.end())
			{
				//log(LogLevel::Trace, fmt::format("Deserialized asset reference: existing asset found [{0}]: Import filepath: {1}\n", assetUuid.getString(), iter->second->getImportFilepath()), "Asset");
				return reinterpret_cast<Asset<T> *>(iter->second);
			}
			
			//log(LogLevel::Trace, fmt::format("Deserialized asset reference: asset not found [{0}]: Creating asset record\n", assetUuid.getString()), "Asset");	

			Asset<T> * newAsset = new Asset<T>(assetUuid);
			m_assets[assetUuid] = newAsset;
			return newAsset;
		}

		template <typename T>
		Asset<T> * getAsset(const UUID & assetUuid) const
		{
			if (!assetUuid.isValid())
				return nullptr;

			auto iter = m_assets.find(assetUuid);

			if (iter == m_assets.end())
			{
				log(LogLevel::Error, fmt::format("Failed to retrieve asset '{0}': Asset not found\n", assetUuid.getString()), "Asset");
				return nullptr;
			}

			Asset<T> * asset = reinterpret_cast<Asset<T> *>(iter->second);

			if (asset == nullptr)
				log(LogLevel::Error, fmt::format("Failed to retrieve asset '{0}': Asset type cast failed\n", assetUuid.getString()), "Asset");

			return asset;
		}

		template<typename T>
		Asset<T> * getAssetFromImportFilepath(const std::string & importFilepath) const
		{
			if (!importFilepath.empty())
				for (const auto & pair : m_assets)
					if (pair.second->getImportFilepath() == importFilepath)
					{
						Asset<T> * asset = reinterpret_cast<Asset<T> *>(pair.second);
						if (asset != nullptr)
							return asset;
						return nullptr;
					}
			return nullptr;
		}

		template <typename T>
		void deleteAsset(Asset<T> *& asset)
		{
			auto iter = m_assets.find(asset->getUuid());

			if (iter == m_assets.end())
			{
				log(LogLevel::Warning, fmt::format("Failed to delete asset '{0}: Asset not found\n", asset->getUuid().getString()), "Asset");
				return;
			}

			asset->deleteLocalFile();

			m_assets.erase(iter);
			delete asset;
			asset = nullptr;
		}

		template <typename T>
		Asset<T> * importAsset(const std::string & importFilepath, const std::string & assetFilepath = "")
		{
			Asset<T> * asset = getAssetFromImportFilepath<T>(importFilepath);

			if (asset != nullptr)
				return asset;

			asset = newAsset<T>();

			if (asset_importer::import<T>(importFilepath, asset, assetFilepath))
				return asset;

			deleteAsset(asset);
			return nullptr;
		}

	private:

		std::string m_filepath;

		std::unordered_map<UUID, AssetBase *> m_assets;
	};

	template <>
	inline const Asset<FontFamily> * AssetManager::getDefaultAsset<FontFamily>()
	{
		static const Asset<FontFamily> * defaultAsset = AssetManager::getInstance().importAsset<FontFamily>("C:/Users/George/Desktop/mud/res/fonts/consola.ttf");
		return defaultAsset;
	}

	template <>
	inline const Asset<Texture> * AssetManager::getDefaultAsset<Texture>()
	{
		static const Asset<Texture> * defaultAsset = AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/white.png");
		return defaultAsset;
	}
}

#endif