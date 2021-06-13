#include "dumper.hpp"

#include <cassert>
#include <sstream>

namespace expurple {

Dumper::Dumper(Json::Whitespace whitespace) noexcept
    : ws(whitespace), indentLevel(0)
{

}

std::string Dumper::dump(const Json &json)
{
    using Type = Json::Type;

    switch (json.type()) {
    case Type::Null:   return "null";
    case Type::Bool:   return json.getBool() ? "true" : "false";
    case Type::Number: return std::to_string(json.getNumber());
    case Type::String: return dumpString(json);
    case Type::Array:  return dumpArray(json);
    case Type::Object: return dumpObject(json);
    }
    assert(false); // function must return inside of switch statement
}

std::string Dumper::dumpString(const Json& json)
{
    // TODO: deal with escape characters properly
    return "\"" + json.getString() + "\"";
}

std::string Dumper::dumpArray(const Json& json)
{
    std::ostringstream result;
    result << "[";
    indentLevel++;
    if (json.size() > 0)
        result << newline() << dump(json[0]);
    for (size_t i = 1; i < json.size(); i++)
        result << "," << newlineOrSpace() << dump(json[i]);
    indentLevel--;
    result << newline() << "]";
    return result.str();
}

std::string Dumper::dumpObject(const Json& json)
{
    // TODO: rewrite to make more efficient
    // as Json gains more methods, e.g. iterators
    auto keys = json.keys();
    auto keyIter = keys.begin();
    std::ostringstream result;
    result << "{";
    indentLevel++;
    if (keyIter != keys.end())
        result << newline()
               << '"' << *keyIter << '"'
               << ':' << space()
               << dump(json.at(*keyIter));
    for (keyIter++; keyIter != keys.end(); keyIter++)
        result << ',' << newlineOrSpace()
               << '"' << *keyIter << '"'
               << ':' << space()
               << dump(json.at(*keyIter));
    indentLevel--;
    result << newline() << "}";
    return result.str();
}

std::string Dumper::newline() const
{
    switch (ws) {
    case Json::Whitespace::None:
    case Json::Whitespace::Space:
        return "";
    case Json::Whitespace::NewlineAndTab:
        return "\n" + std::string(indentLevel, '\t');
    }
    assert(false); // function must return inside of switch statement
}

std::string Dumper::newlineOrSpace() const
{
    switch (ws) {
    case Json::Whitespace::None:
        return "";
    case Json::Whitespace::Space:
        return " ";
    case Json::Whitespace::NewlineAndTab:
        return "\n" + std::string(indentLevel, '\t');
    }
    assert(false); // function must return inside of switch statement
}

std::string Dumper::space() const
{
    switch (ws) {
    case Json::Whitespace::None:
        return "";
    case Json::Whitespace::Space:
    case Json::Whitespace::NewlineAndTab:
        return " ";
    }
    assert(false); // function must return inside of switch statement
}

} // end of namespace "expurple"
