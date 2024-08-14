#ifndef JSONSTRUCTUREEXCEPTION_HPP
#define JSONSTRUCTUREEXCEPTION_HPP

#include <stdexcept>
#include <string>

// Custom exception class for JSON structure errors
class JsonStructureException : public std::runtime_error {
public:
    explicit JsonStructureException(const std::string& message)
            : std::runtime_error(message) {}
};

#endif // JSONSTRUCTUREEXCEPTION_HPP