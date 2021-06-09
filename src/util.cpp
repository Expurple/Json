#include "util.hpp"

#include <cassert>
#include <cstring>

#include "json.hpp"

namespace expurple {

// declated in class Json as friend
Json::TypeError typeError(const char* fmt, Json::Type thisType)
{
    assert(strstr(fmt, "%s") != nullptr);

    using Type = Json::Type;
    const char* typeStr;
    switch (thisType) {
    case Type::Null:   typeStr = "Null";   break;
    case Type::Bool:   typeStr = "Bool";   break;
    case Type::Number: typeStr = "Nubmer"; break;
    case Type::String: typeStr = "String"; break;
    case Type::Array:  typeStr = "Array";  break;
    case Type::Object: typeStr = "Object"; break;
    }
    std::string msg = format(fmt, typeStr);
    return Json::TypeError(msg);
}

} // end of namespace "expurple"
