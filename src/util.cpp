#include "util.hpp"

#include <cassert>

namespace expurple {

using Type = Json::Type;

const char* toString(Type type)
{
    switch (type) {
    case Type::Null:   return "Null";
    case Type::Bool:   return "Bool";
    case Type::Number: return "Number";
    case Type::String: return "String";
    case Type::Array:  return "Array";
    case Type::Object: return "Object";
    }
    assert(false); // function must return inside of switch statement
}

} // end of namespace "expurple"
