#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>

class liscppError : public std::runtime_error {
    std::string msg;
public:
    explicit liscppError(const std::string& arg);
};

class syntaxError : public liscppError {
public:
    explicit syntaxError(const std::string& arg);
};

class typeError : public liscppError {
public:
    explicit typeError(const std::string& arg);
};

class valueError : public liscppError {
public:
    explicit valueError(const std::string& arg);
};

class argInvalidError : public liscppError {
public:
    explicit argInvalidError(const std::string& arg);
};

class IOError : public liscppError {
public:
    explicit IOError(const std::string& arg);
};

class REPLError : public liscppError {
public:
    explicit REPLError(const std::string& arg);
};

#endif //ERROR_H
