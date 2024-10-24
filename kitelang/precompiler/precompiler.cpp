#include "precompiler.h"

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
            if (command == "define") {
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
                handle_define(key, value);
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
            pos += value.length();
        }
    }

    return result;
}

void Precompiler::handle_define(const std::string& key, const std::string& value) {
    definitions[key] = value;
}
