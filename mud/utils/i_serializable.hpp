#ifndef I_SERIALIZABLE_HPP
#define I_SERIALIZABLE_HPP

#include <fstream>
#include <string>

#include "serialization_helpers.hpp"

namespace mud
{
	class ISerializable
	{
	public:

		virtual ~ISerializable() { };

		virtual bool deserialize(std::ifstream & file) = 0;

		virtual bool serialize(std::ofstream & file) const = 0;
	};
}

#endif