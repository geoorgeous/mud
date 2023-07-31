#include "file_io.hpp"

#include <fstream>
#include <sstream>

#include <fmt/format.h>

#include "logger.hpp"

namespace mud::file {

	std::string readText(const std::string & filepath)
	{
		std::ifstream file(filepath);

		if (file.fail())
		{
			log(LogLevel::Error, fmt::format("Failed to open file '{0}' for read\n", filepath), "File IO");
			return "";
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		return buffer.str();
	}

    bool writeBinary(const std::string & destination, const std::vector<uint32_t> & data)
    {
        std::ofstream file(destination, std::ios::out | std::ios::binary);

		if (file.fail())
		{
			log(LogLevel::Error, fmt::format("Failed to open file '{0}' for write\n", destination), "File IO");
			return false;
		}

		file.write(reinterpret_cast<char *>(&(const_cast<std::vector<uint32_t> &>(data))[0]), sizeof(uint32_t) * data.size());

		if (!file.good())
		{
			log(LogLevel::Error, fmt::format("Error while writing to file '{0}'\n", destination), "File IO");
			return false;
		}

        return true;
    }
}