#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

namespace errors {
    class kiterr : public std::exception {
    private:
        std::string message;

    public:
        int line, pos_start, pos_end;
        kiterr(const std::string& msg, int line, int pos_start, int pos_end) : message(msg), line(line), pos_start(pos_start), pos_end(pos_end) {}
        virtual const char* what() const noexcept override {
            return message.c_str();
        }
    };
}