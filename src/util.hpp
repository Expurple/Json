#ifndef EXPURPLE_JSON_UTIL_HPP
#define EXPURPLE_JSON_UTIL_HPP

#include <cstdio>

#include "json.hpp"

namespace expurple
{

// Waiting for C++20...
// This is very unsafe, but should do the job for short messages
template<size_t buffSize = 300, class... Args>
std::string format(const char* fmt, const Args&... args)
{
    std::string buf;
    buf.reserve(buffSize);
    sprintf(&buf[0], fmt, args...);
    return buf;
}

} // end of namespace "expurple"

const char* toString(expurple::Json::Type type);

#endif // EXPURPLE_JSON_UTIL_HPP
