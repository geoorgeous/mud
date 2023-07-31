#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <unordered_map>

#include "math/vector.hpp"
#include "texture.hpp"
#include "utils/asset_object.hpp"
#include "utils/asset.hpp"
#include "utils/i_serializable.hpp"

namespace mud
{
	struct Material : public AssetObject<Material>
	{
		Vector4 baseColor;
		const Asset<Texture> * diffuseMap;
		const Asset<Texture> * normalMap;
		const Asset<Texture> * metalnessMap;
		const Asset<Texture> * roughnessMap;

		virtual bool deserialize(std::ifstream & file) override;

		virtual bool serialize(std::ofstream & file) const override;
	};

	template<>
	inline AssetObject<Material>::AssetObject()
		: AssetObjectBase(AssetObjectType::Material)
	{ }
}

#endif