#ifndef ASSET_HPP
#define ASSET_HPP

#include <fstream>
#include <string>

#include "asset_object.hpp"
#include "uuid.hpp"

namespace mud
{
	class AssetObjectBase;

	struct AssetMetaData
	{
		std::string header;
		AssetObjectType type;
		UUID uuid;
		std::string importFilepath;
	};

	class AssetBase
	{
	public:

		AssetBase();

		AssetBase(const UUID & uuid);

		~AssetBase();

		void unload();

		const UUID & getUuid() const;

		const std::string & getFilepath() const;

		const std::string & getImportFilepath() const;

		virtual void setImportFilepath(const std::string & filepath);
		
		bool allocateObject();

		void move(const std::string & filepath);

		bool save();

		bool save(const std::string & filepath);

		bool load(bool loadAssetObject = true);

		bool load(const std::string & filepath, bool loadAssetObject = true);

		bool isObjectLoaded() const;

		bool deleteLocalFile();

		bool serializeReference(std::ofstream & file) const;

		static bool deserializeMetaDataFromFile(const std::string & filepath, AssetMetaData & metaData);

	protected:

		mutable AssetObjectBase * m_object;

		virtual bool allocateObjectInternal() const = 0;

		static bool deserializeMetaData(std::ifstream & file, const std::string & filepath, AssetMetaData & metaData);

		bool deserializeObject() const;

	private:

		static const std::string expectedHeaderContent;

		UUID m_uuid;
		std::string m_filepath;
		std::string m_importFilepath;

		bool tryOpenIFile(std::ifstream & file) const;
	};

	template <typename T>
	class Asset : public AssetBase
	{
	public:

		Asset()
			: AssetBase()
		{ }

		Asset(const UUID & uuid)
			: AssetBase(uuid)
		{ }

		const T * const get() const
		{
			return getInternal();
		}

		T * get()
		{
			return getInternal();
		}

	private:
	
		virtual bool allocateObjectInternal() const override
		{
			if (m_object == nullptr)
				m_object = new T();
			return m_object != nullptr;
		}

		T * getInternal() const
		{
			if (!isObjectLoaded())
				deserializeObject();
			if (!isObjectLoaded())
				return nullptr;
			return reinterpret_cast<T *>(m_object);
		}
	};
}

#endif