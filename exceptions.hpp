#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Custom exception class for JSON structure errors
class JsonStructureException : public std::runtime_error {
public:
    explicit JsonStructureException(const std::string& message)
            : std::runtime_error(message) {}
};

#endif // EXCEPTIONS_HPP