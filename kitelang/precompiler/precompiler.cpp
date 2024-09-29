#include "precompiler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <filesystem>

namespace fs = std::filesystem;

std::string Precompiler::precompile(const std::string& source) {
    std::string result;

    for (size_t ptr = 0; ptr < source.size(); ptr++) {
        if (source[ptr] == '#') {
            ptr++;
            std::string command;
            while (ptr < source.size() && isalpha(source[ptr])) {
                command += source[ptr++];
            }

            while (ptr < source.size() && isspace(source[ptr])) {
                ptr++;
            }

            std::string argument;
            if (command == "include") {
                char delimiter = source[ptr];
                if (delimiter == '"' || delimiter == '<') {
                    ptr++;
                    while (ptr < source.size() && source[ptr] != '"' && source[ptr] != '>') {
                        argument += source[ptr++];
                    }
                    if (ptr < source.size()) ptr++;
                    result += handleInclude(argument, delimiter);
                }
            }
            else if (command == "define") {
                while (ptr < source.size() && isspace(source[ptr])) {
                    ptr++;
                }
                std::string key, value;
                while (ptr < source.size() && !isspace(source[ptr])) {
                    key += source[ptr++];
                }
                while (ptr < source.size() && isspace(source[ptr])) {
                    ptr++;
                }
                while (ptr < source.size() && !isspace(source[ptr])) {
                    value += source[ptr++];
                }
                handleDefine(key, value);
            }
            while (ptr < source.size() && source[ptr] != '\n') {
                result += source[ptr++];
            }
            result += '\n';
        }
        else {
            result += source[ptr];
        }
    }

    for (const auto& pair : definitions) {
        std::string key = pair.first;
        std::string value = pair.second;
        size_t pos = 0;
        while ((pos = result.find(key, pos)) != std::string::npos) {
            result.replace(pos, key.length(), value);
            pos += value.length(); // Move past the replacement
        }
    }

    return result;
}

std::string Precompiler::handleInclude(const std::string& filename, char type) {
    std::string normalizedFilename = fs::path(filename).filename().string();

    if (includedFiles.find(normalizedFilename) != includedFiles.end()) {
        std::cout << "Skipping already included file: " << normalizedFilename << std::endl;
        return "";
    }

    includedFiles.insert(normalizedFilename);

    std::string path;

    if (!filename.empty()) {
        if (type == '<') {
            path = "stdinclude/" + normalizedFilename;
        }
        else if (type == '"')
            path = normalizedFilename;
        else throw std::runtime_error("Invalid include format for " + filename);
    }
    else throw std::runtime_error("Empty @include precompiler directive");

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file " + path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Precompiler::handleDefine(const std::string& key, const std::string& value) {
    definitions[key] = value;
}
