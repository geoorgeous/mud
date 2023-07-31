#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <string>
#include <vector>

namespace mud::file {

	std::string readText(const std::string & filepath);

    bool writeBinary(const std::string & destination, const std::vector<uint32_t> & data);
}

#endif