#pragma once

#include <string>
#include <unordered_map>

class EnvReader {
public:
	explicit EnvReader(const std::string& filePath);

	std::string operator[] (const std::string& key) const;
private:
	std::unordered_map<std::string, std::string> mMappings;
};