#include "util.hpp"

#include "json.hpp"

namespace expurple {

// declated in class Json as friend.
// fmt must contain "%s" in place of Type
Json::TypeError typeError(const char* fmt, Json::Type thisType)
{
    using Type = Json::Type;
    static const std::unordered_map<Type, const char*> typeStrings = {
        {Type::Null,   "Null"},
        {Type::Bool,   "Bool"},
        {Type::Number, "Number"},
        {Type::String, "String"},
        {Type::Array,  "Array"},
        {Type::Object, "Object"}
    };
    return Json::TypeError(format(fmt, typeStrings.at(thisType)));
}

} // end of namespace "expurple"
