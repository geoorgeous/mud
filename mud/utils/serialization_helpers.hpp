#ifndef SERIALIZATION_HELPERS_HPP
#define SERIALIZATION_HELPERS_HPP

#include <fstream>
#include <string>
#include <vector>

#include "utils/logger.hpp"

namespace mud::serialization_helpers
{
	template <typename T>
	bool serialize(std::ofstream & file, const T * p, size_t size)
	{
		file.write(reinterpret_cast<const char *>(p), size);
		return file.good();
	}

	template <typename T>
	bool serialize(std::ofstream & file, const T & pod)
	{
		return serialize(file, &pod, sizeof(T));
	}

	template <>
	inline bool serialize<std::string>(std::ofstream & file, const std::string & string)
	{
		!serialization_helpers::serialize(file, string.length());
		if (string.length() > 0)
			file.write(reinterpret_cast<const char *>(&string[0]), sizeof(char) * string.length());
		return file.good();
	}

	template <typename VectorT>
	bool serializeVector(std::ofstream & file, const std::vector<VectorT> & vector)
	{
		serialization_helpers::serialize(file, vector.size());
		if (vector.size() > 0)
			file.write(reinterpret_cast<const char *>(&vector[0]), sizeof(VectorT) * vector.size());
		return file.good();
	}

	template <typename T>
	bool deserialize(std::ifstream & file, T * p, size_t size)
	{
		file.read(reinterpret_cast<char *>(p), size);
		return file.good();
	}

	template <typename T>
	bool deserialize(std::ifstream & file, T & pod)
	{
		return deserialize(file, &pod, sizeof(T));
	}

	template <>
	inline bool deserialize<std::string>(std::ifstream & file, std::string & string)
	{
		size_t length = 0;
		serialization_helpers::deserialize(file, length);
		string.resize(length);
		if (length != 0)
			file.read(reinterpret_cast<char *>(&string[0]), sizeof(char) * string.size());
		return file.good();
	}

	template <typename VectorT>
	bool deserializeVector(std::ifstream & file, std::vector<VectorT> & vector)
	{
		size_t size = 0;
		serialization_helpers::deserialize(file, size);
		vector.resize(size);
		if (size != 0)
			file.read(reinterpret_cast<char *>(&vector[0]), sizeof(VectorT) * vector.size());
		return file.good();
	}
}

#endif