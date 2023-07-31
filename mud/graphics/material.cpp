#include "material.hpp"

#include <string.h>

#include "utils/asset_manager.hpp"

namespace mud
{
	bool Material::deserialize(std::ifstream & file)
	{
		//log(LogLevel::Trace, "Deserializing Material...\n", "Asset");
		serialization_helpers::deserialize(file, baseColor);
		diffuseMap = AssetManager::getInstance().deserializeAssetReference<Texture>(file);
		normalMap = AssetManager::getInstance().deserializeAssetReference<Texture>(file);
		metalnessMap = AssetManager::getInstance().deserializeAssetReference<Texture>(file);
		roughnessMap = AssetManager::getInstance().deserializeAssetReference<Texture>(file);

		return true;
	}

	bool Material::serialize(std::ofstream & file) const
	{
		//log(LogLevel::Trace, "Serializing Material...\n", "Asset");
		serialization_helpers::serialize(file, baseColor);
		diffuseMap->serializeReference(file);
		normalMap->serializeReference(file);
		metalnessMap->serializeReference(file);
		roughnessMap->serializeReference(file);

		return true;
	}
}