#include "uuid.hpp"

#include <random>
#include <sstream>
#include <string.h>

namespace mud
{
	static std::random_device randomDevice;
	static std::mt19937_64 generator(randomDevice());
	static std::uniform_int_distribution<> dis(0, 15);
	static std::uniform_int_distribution<> dis2(8, 11);

	uint32_t UUID::length = 36;

	UUID::UUID()
	{ }

	UUID::UUID(const std::string & uuid)
		: m_uuid(uuid)
	{ }

	const std::string & UUID::getString() const
	{
		return m_uuid;
	}

	void UUID::generate()
	{
		std::stringstream ss;
		int idx;
		ss << std::hex;

		for (idx = 0; idx < 8; idx++)
			ss << dis(generator);

		ss << "-";
		for (idx = 0; idx < 4; idx++)
			ss << dis(generator);

		ss << "-4";
		for (idx = 0; idx < 3; idx++)
			ss << dis(generator);

		ss << "-";
		ss << dis2(generator);
		for (idx = 0; idx < 3; idx++)
			ss << dis(generator);

		ss << "-";
		for (idx = 0; idx < 12; idx++)
			ss << dis(generator);

		m_uuid = ss.str();
	}

	bool UUID::isValid() const
	{
		return m_uuid.length() == UUID::length && m_uuid[8] == '-' && m_uuid[13] == '-' && m_uuid[18] == '-' && m_uuid[23] == '-';
	}

	bool UUID::operator==(const UUID & other) const
	{
		return m_uuid == other.m_uuid;
	}

	bool UUID::deserialize(std::ifstream & file)
	{
		m_uuid = "";

		uint8_t isValidByte = 0;
		serialization_helpers::deserialize(file, isValidByte);

		if (isValidByte == 1u)
			serialization_helpers::deserialize(file, m_uuid);

		return true;
	}

	bool UUID::serialize(std::ofstream & file) const
	{
		serialization_helpers::serialize(file, uint8_t(isValid() ? 1u : 0u));

		if (isValid())
			serialization_helpers::serialize(file, m_uuid);

		return true;
	}
}