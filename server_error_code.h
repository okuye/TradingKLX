#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
#pragma once

#include <cstdint>
#include <system_error>  // For std::error_code and std::error_category
#include <type_traits>   // For std::true_type

namespace mongocxx {

    enum class server_error_code : std::int32_t {
        // Add your error codes here
    };

}  // namespace mongocxx

namespace std {

    template <>
    struct is_error_code_enum<mongocxx::server_error_code> : public true_type {};

}  // namespace std

namespace mongocxx {

    std::error_code make_error_code(server_error_code error);

}  // namespace mongocxx
