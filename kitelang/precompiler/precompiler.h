#pragma once

#include <string>
#include <unordered_map>
#include <set>

class Precompiler {
public:
    std::string precompile(const std::string& source);

private:
    std::set<std::string> loadedkms {};
    std::unordered_map<std::string, std::string> definitions {};

    std::string handleInclude(const std::string&, char);
    void handleDefine(const std::string& key, const std::string& value);
};