#ifndef ASSET_IMPORTER_HPP
#define ASSET_IMPORTER_HPP

#include <string>

#include "asset.hpp"
#include "graphics/font.hpp"
#include "graphics/scene_graph.hpp"
#include "graphics/texture.hpp"

namespace mud
{
	class AssetManager;

	namespace asset_importer
	{
		template<typename T>
		bool import(const std::string & filepath, Asset<T> * asset, const std::string & assetFilepath = "")
		{
			return false;
		}

		template<>
		bool import<SceneGraph>(const std::string & filepath, Asset<SceneGraph> * asset, const std::string & assetFilepath);

		template<>
		bool import<FontFamily>(const std::string & filepath, Asset<FontFamily> * asset, const std::string & assetFilepath);

		template<>
		bool import<Texture>(const std::string & filepath, Asset<Texture> * asset, const std::string & assetFilepath);
	}
}

#endif