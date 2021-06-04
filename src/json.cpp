#include "json.hpp"

#include <assert.h>

namespace expurple {

Json::Json()
    : value(Object())
{

}

Json::Json(const Json& other)
{
#ifdef EXPURPLE_JSON_DEBUG_MOVE
    if (other.type() == Type::Moved)
        throw MoveError("Tried to copy a moved-from Json object");
#endif
    value = copy(other);
}

Json::Json(Json&& other)
{
#ifdef EXPURPLE_JSON_DEBUG_MOVE
    if (other.type() == Type::Moved)
        throw MoveError("Tried to move a moved-from Json object twice");
#endif
    value = std::move(other.value);
    other.value = Moved();
}

Json::Json(std::nullptr_t) noexcept
    : value(nullptr)
{

}

Json::Json(bool val) noexcept
    : value(val)
{

}

Json::Json(const char *val)
    : value(std::string(val))
{

}

Json::Json(Array&& val)
    : value(std::move(val))
{

}

Json::Json(Object&& val)
    : value(std::move(val))
{

}

Json& Json::operator=(const Json& other)
{
#ifdef EXPURPLE_JSON_DEBUG_MOVE
    if (other.type() == Type::Moved)
        throw MoveError("Tried to copy a moved-from Json object");
#endif
    if (&other != this)
        value = copy(other);
    return *this;
}

Json& Json::operator=(Json&& other)
{
#ifdef EXPURPLE_JSON_DEBUG_MOVE
    if (other.type() == Type::Moved)
        throw MoveError("Tried to move a moved-from Json object twice");
#endif
    assert(&other != this);
    value = std::move(other.value);
    other.value = Moved();
    return *this;
}

Json::Array Json::copy(const Array& array)
{
    Array copy;
    copy.reserve(array.size());
    for (const auto& val : array)
        copy.emplace_back(new Json(*val));
    return copy;
}

Json::Object Json::copy(const Object& object)
{
    Object copy;
    copy.reserve(object.size());
    for (const auto& [key, val] : object)
        copy.emplace(key, new Json(*val));
    return copy;
}

Json::Value Json::copy(const Json& json)
{
    // Implicit constructors all the way
    switch (json.type()) {
    case Type::Null:
        return nullptr;
    case Type::Bool:
        return std::get<bool>(json.value);
    case Type::Number:
        return std::get<double>(json.value);
    case Type::String:
        return std::get<std::string>(json.value);
    case Type::Array: {
        const auto& arrayValue = std::get<Array>(json.value);
        return std::move(copy(arrayValue)); }
    case Type::Object: {
        const auto& objectValue = std::get<Object>(json.value);
        return std::move(copy(objectValue)); }
    case Type::Moved:
#ifdef EXPURPLE_JSON_DEBUG_MOVE
        throw MoveError("Tried to copy a moved-from Json object");
#else
        return Moved();
#endif
    }
    throw std::logic_error("Function must return inside switch");
}

Json::Type Json::type() const
{
    // IMPORTANT: relies on enum values and std::variant indexes being in the same order
    return static_cast<Type>(value.index());
};

// non-member operators:

bool Json::areEqual(const Json::Array& left, const Json::Array& right)
{
    if (left.size() != right.size())
        return false;
    for (size_t i = 0; i < left.size(); ++i)
        if (*left[i] != *right[i])
            return false;
    return true;
}

bool Json::areEqual(const Json::Object& left, const Json::Object& right)
{
    if (left.size() != right.size())
        return false;
    for (const auto& [leftKey, leftVal] : left)
    {
        auto rightIter = right.find(leftKey);
        if (rightIter == right.end())
            return false;
        if (*leftVal != *rightIter->second)
            return false;
    }
    return true;
}

bool operator==(const Json& left, const Json& right)
{
    using Type = Json::Type;
#ifdef EXPURPLE_JSON_DEBUG_MOVE
    if (right.type() == Type::Moved || left.type() == Type::Moved)
        throw Json::MoveError("Tried to use a moved-from Json object");
#endif
    if (left.type() != right.type())
        return false;

    switch (left.type()) {
    case Type::Null:
        return true;
    case Type::Bool:
    case Type::Number:
    case Type::String:
        return left.value == right.value;
    case Type::Array:
        return Json::areEqual(std::get<Json::Array>(left.value),
                        std::get<Json::Array>(right.value));
    case Type::Object:
        return Json::areEqual(std::get<Json::Object>(left.value),
                        std::get<Json::Object>(right.value));
    case Type::Moved:
#ifdef EXPURPLE_JSON_DEBUG_MOVE
        throw std::logic_error("Should be already checked before switch");
#else
        return true;
#endif
    }
    throw std::logic_error("Function must return earlier");
}

bool operator!=(const Json& left, const Json& right)
{
    return !(left == right);
}

} // end of namespace "expurple"
