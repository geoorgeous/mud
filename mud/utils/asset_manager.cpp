#include "asset_manager.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>

#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/scene_graph.hpp"
#include "graphics/texture.hpp"

namespace mud
{
	const std::string AssetManager::assetDirectory = ".\\assets";
	const std::string AssetManager::assetFileExtension = ".masset";

	AssetManager & AssetManager::getInstance()
	{
		static AssetManager assetManager;
		return assetManager;
	}

	std::string AssetManager::createAssetFilepath(const std::string & relativeFilepath)
	{
		return assetDirectory + "/" + relativeFilepath + assetFileExtension;
	}

	AssetManager::AssetManager()
		: m_filepath("mud.assets")
	{ }

	AssetManager::~AssetManager()
	{
		for (auto & pair : m_assets)
			delete pair.second;
	}

	bool filepathExists(const std::unordered_map<UUID, AssetBase *> & m_assets, const std::string & filepath)
	{
		for (const auto & pair : m_assets)
			if (pair.second->getFilepath() == filepath)
				return true;
		return false;
	}

	void AssetManager::makeFilepathUnique(std::string & filepath) const
	{
		if (!filepathExists(m_assets, filepath))
			return;

		auto path = std::filesystem::path(filepath);

		int count = 0;

		do
		{
			count++;
			filepath = path.parent_path().string() + "/" + path.stem().string() + fmt::format(" ({0})", count) + path.extension().string();
		} while (filepathExists(m_assets, filepath));
	}

	std::string AssetManager::createUniqueAssetFilepath(const std::string & relativeFilepath)
	{
		std::string filepath = AssetManager::createAssetFilepath(relativeFilepath);
		makeFilepathUnique(filepath);
		return filepath;
	}

	void AssetManager::importLocalAssets()
	{
		const size_t numAssetsBefore = m_assets.size();

		if (std::filesystem::is_directory(assetDirectory))
			for (const auto & directoryEntry : std::filesystem::recursive_directory_iterator(assetDirectory))
			{
				const std::filesystem::path & path = directoryEntry.path();

				if (std::filesystem::is_regular_file(path) && path.extension().string() == ".masset")
				{
					AssetBase * asset = nullptr;

					const std::string assetFilepath = path.string();
					
					AssetMetaData metaData;
					if (!AssetBase::deserializeMetaDataFromFile(assetFilepath, metaData))
						continue;

					auto existingAsset = m_assets.find(metaData.uuid);

					if (existingAsset != m_assets.end())
						asset = existingAsset->second;
					else
					{
						switch (metaData.type)
						{
						default:
						case AssetObjectType::Unsupported:
							log(LogLevel::Error, fmt::format("Failed to load local asset file '{0}': unexpected asset type '{1}' (UUID: {2})\n", assetFilepath, static_cast<uint32_t>(metaData.type), metaData.uuid.getString()), "Asset");
							break;

						case AssetObjectType::FontFamily:
							asset = new Asset<FontFamily>();
							break;

						case AssetObjectType::Material:
							asset = new Asset<Material>();
							break;

						case AssetObjectType::Mesh:
							asset = new Asset<Mesh>();
							break;

						case AssetObjectType::SceneGraph:
							asset = new Asset<SceneGraph>();
							break;

						case AssetObjectType::Texture:
							asset = new Asset<Texture>();
							break;
						}

						if (asset == nullptr)
							continue;
					}

					if (asset->load(assetFilepath, false));
					{
						//log(LogLevel::Trace, fmt::format("Local asset found '{0}': ({1})[{2}] '{3}'\n", asset->getFilepath(), static_cast<int>(metaData.type), metaData.uuid.getString(), metaData.importFilepath), "Asset");
						m_assets[asset->getUuid()] = asset;
					}
				}
			}

		//log(LogLevel::Trace, fmt::format("Finished importing local assets. {0} asset(s) found\n", m_assets.size() - numAssetsBefore), "Asset");
	}

	void AssetManager::unloadAssets()
	{
		for (auto & pair : m_assets)
			pair.second->unload();
	}
	
	bool AssetManager::importAssetUnknownType(const std::string & filepath)
	{
		const std::filesystem::path path(filepath);

		if (!path.has_extension())
		{
			log(LogLevel::Error, fmt::format("Failed to import asset file '{0}': Could not determine file type - missing file extension", filepath));
			return false;
		}

		AssetBase * asset;

		std::string ext = std::filesystem::path(filepath).extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });

		if (ext == ".ttf")
			return importAsset<FontFamily>(filepath) != nullptr;

		if (ext == ".obj" ||
			ext == ".gltf" ||
			ext == ".glb" ||
			ext == ".fbx" ||
			ext == ".dae")
			return importAsset<SceneGraph>(filepath) != nullptr;

		if (ext == ".png" ||
			ext == ".jpg" ||
			ext == ".jpeg" ||
			ext == ".bmp" ||
			ext == ".tga" ||
			ext == ".stl")
			return importAsset<Texture>(filepath) != nullptr;
			
		log(LogLevel::Error, fmt::format("Failed to import asset file '{0}': Could not determine file type - unsupported file extension '{1}'", filepath, ext));
		return false;
	}

	bool AssetManager::importDirectory(const std::string & directory)
	{
		if (!std::filesystem::is_directory(directory))
		{
			log(LogLevel::Error, fmt::format("Failed to import directory '{0}': Provided path is not a directory", directory));
			return false;
		}

		bool allSuccess = true;
		
		for (const auto & directoryEntry : std::filesystem::recursive_directory_iterator(directory))
			allSuccess = importAssetUnknownType(directoryEntry.path().string()) && allSuccess;

		return allSuccess;
	}
}