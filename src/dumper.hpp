#ifndef EXPURPLE_JSON_DUMPER_HPP
#define EXPURPLE_JSON_DUMPER_HPP

#include "json.hpp"

namespace expurple {

class Dumper
{
public:
    Dumper(Json::Whitespace whitespace) noexcept;
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
};

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_DUMPER_HPP
