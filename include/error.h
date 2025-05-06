#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>

class syntaxError : public std::runtime_error {
    std::string msg;
public:
    explicit syntaxError(const std::string& arg);
};

class typeError : public std::runtime_error {
    std::string msg;
public:
    explicit typeError(const std::string& arg);
};

class valueError : public std::runtime_error {
    std::string msg;
public:
    explicit valueError(const std::string& arg);
};

class argInvalidError : public std::runtime_error {
    std::string msg;
public:
    explicit argInvalidError(const std::string& arg);
};

#endif //ERROR_H
