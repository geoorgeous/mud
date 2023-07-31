#ifndef UUID_HPP
#define UUID_HPP

#include <string>

#include "i_serializable.hpp"

namespace mud
{
	class UUID : public ISerializable
	{
	public:

		static uint32_t length;

		UUID();

		UUID(const std::string & uuid);

		const std::string & getString() const;

		void generate();

		bool isValid() const;

		bool operator==(const UUID & other) const;

		virtual bool deserialize(std::ifstream & file) override;

		virtual bool serialize(std::ofstream & file) const override;

	private:

		std::string m_uuid;
	};
}

namespace std
{
	template <>
	struct hash<mud::UUID>
	{
		std::size_t operator()(const mud::UUID & uuid) const
		{
			hash<string> stringHasher;
			return stringHasher(uuid.getString());
		}
	};
}

#endif