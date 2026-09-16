#include "env_reader.hpp"

#include <string>
#include <fstream>
#include <stdexcept>

EnvReader::EnvReader(const std::string& filePath) {
	std::ifstream file{filePath};

	if (!file) throw std::runtime_error("Failed to locate/open .env file.");

	std::string line;

	while (std::getline(file, line)) {
		size_t pos = line.find('=');

		if (pos == std::string::npos) continue;

		mMappings[line.substr(0, pos)] = line.substr(pos + 1);
	}
}

std::string EnvReader::operator[](const std::string& key) const {
	auto it = mMappings.find(key);

	if (it == mMappings.end())
		throw std::runtime_error("Environment variable not found: " + key);

	return it->second;
}