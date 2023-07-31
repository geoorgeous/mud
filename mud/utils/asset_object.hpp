#ifndef ASSET_OBJECT_HPP
#define ASSET_OBJECT_HPP

#include "i_serializable.hpp"

namespace mud
{
	enum class AssetObjectType
	{
		Unsupported,
		FontFamily,
		Material,
		Mesh,
		SceneGraph,
		Texture
	};

	class AssetObjectBase : public ISerializable
	{
	public:

		AssetObjectType getType() const
		{
			return m_type;
		}

	protected:

		const AssetObjectType m_type;

		AssetObjectBase(AssetObjectType type)
			: m_type(type)
		{ }
	};

	template <typename T>
	class AssetObject : public AssetObjectBase
	{
	public:

		AssetObject()
			: AssetObjectBase()
		{ }

		const T * get() const
		{
			return reinterpret_cast<const T *>(this);
		}

		T * get()
		{
			return reinterpret_cast<T *>(this);
		}
	};
}

#endif