#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <filesystem>

namespace fs = std::filesystem;

class Precompiler {
public:
    std::string precompile(const std::string& source);

private:
    std::unordered_map<std::string, std::string> definitions{};
    void handle_define(const std::string& key, const std::string& value);
};
