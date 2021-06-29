#ifndef EXPURPLE_JSON_DUMPER_HPP
#define EXPURPLE_JSON_DUMPER_HPP

#include <unordered_map>

#include "json.hpp"

namespace expurple {

// A temporary object for each call of Json::toString() or Json::operator<<,
// holding some internal state while doing all the stringification work.
class Dumper
{
public:
    explicit Dumper(Json::Whitespace whitespace) noexcept;
    std::string dump(const Json& json);
private:
    std::string dumpString(const Json& json);
    std::string dumpArray(const Json& json);
    std::string dumpObject(const Json& json);

    std::string newline() const;
    std::string newlineOrSpace() const;
    std::string space() const;

// fields:
    const Json::Whitespace ws;
    size_t indentLevel;

    static const std::unordered_map<char, char> escapeChars;
};

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_DUMPER_HPP
