#include "json.hpp"

#include <cassert>

#include "parser.hpp"
#include "util.hpp"

namespace expurple {

Json::Json()
    : value(Object())
{

}

Json::Json(const Json& other)
{
    value = copy(other);
}

Json::Json(Json&& other)
{
    value = std::move(other.value);
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

Json::Json(const std::string& val)
    : value(val)
{

}

Json::Json(std::string&& val) noexcept
    : value(val)
{

}

Json::Json(Array&& val) noexcept
    : value(std::move(val))
{

}

Json::Json(Object&& val)
    : value(std::move(val))
{

}

Json& Json::operator=(const Json& other)
{
    if (&other != this)
        value = copy(other);
    return *this;
}

Json& Json::operator=(Json&& other)
{
    assert(&other != this);
    value = std::move(other.value);
    return *this;
}

Json& Json::operator[](const std::string& key)
{
    if (type() != Type::Object)
    {
        auto fmt = "Can't index %s with std::string: not an Object";
        throw typeError(fmt, type());
    }

    auto& map = std::get<Object>(value);
    auto iter = map.find(key);
    if (iter == map.end())
    {
        auto newNode = std::pair {key, new Json()};
        iter = map.insert(newNode).first;
    }
    return *iter->second;
}

Json& Json::operator[](size_t index)
{
    if (type() != Type::Array)
        throw typeError("Can't index %s with a number: not an Array", type());

    size_t size = std::get<Array>(value).size();
    if (index >= size)
    {
        auto fmt = "Index %u is out of bounds (size of Array is %u)";
        throw IndexError(format(fmt, index, size));
    }

    return *std::get<Array>(value)[index];
}

Json Json::array()
{
    return Array();
}

Json Json::parse(const char* str, WhitespaceAfter wsafter, DuplicateKeys dkeys)
{
    assert(str != nullptr);

    std::istringstream stream(str);
    Parser p(stream, wsafter, dkeys);
    return p.parse();
}

Json Json::parse(const std::string& str, WhitespaceAfter wsafter, DuplicateKeys dkeys)
{
    std::istringstream stream(str);
    Parser p(stream, wsafter, dkeys);
    return p.parse();
}

Json Json::parse(std::istream &istream, WhitespaceAfter wsafter, DuplicateKeys dkeys)
{
    Parser p(istream, wsafter, dkeys);
    return p.parse();
}

Json& Json::at(const std::string& key)
{
    switch (type()) {
    case Type::Null:
    case Type::Bool:
    case Type::Number:
        throw typeError("Can't call 'at()' on a %s value", type());
    case Type::String:
        throw TypeError("Can't call 'at()' on a String value."
                        " Try to use 'json.getString().at()'");
    case Type::Array:
        throw TypeError("Can't call 'at(const std::string&)' on an Array value."
                        " Try to use 'at(size_t)'");
    case Type::Object:
        try {
            return *std::get<Object>(value).at(key); }
        catch (const std::out_of_range&) {
            throw KeyError("Tried to access a non-existing property '"+ key + "'"); }
    }
    throw std::logic_error("Function must return inside switch");
}

Json& Json::at(size_t index)
{
    switch (type()) {
    case Type::Null:
    case Type::Bool:
    case Type::Number:
        throw typeError("Can't call 'at()' on a %s value", type());
    case Type::String:
        throw TypeError("Can't call 'at()' on a String value."
                        " Try to use 'json.getString().at()'");
    case Type::Array:
        return (*this)[index]; // operator[](size_t) does bound checking too
    case Type::Object:
        throw TypeError("Can't call 'at(size_t)' on an Object value."
                        " Try to use 'at(const std::string&)'");
    }
    throw std::logic_error("Function must return inside switch");
}

void Json::push_back(const Json& val)
{
    if (type() != Type::Array)
    {
        auto fmt = "Can't call 'push_back()' on a %s value: not an Array";
        throw typeError(fmt, type());
    }

    std::get<Array>(value).emplace_back(new Json(val));
}

bool Json::getBool() const
{
    if (type() != Type::Bool)
    {
        auto fmt = "Can't call 'getBool()' on a %s value: not a Bool";
        throw typeError(fmt, type());
    }

    return std::get<bool>(value);
}

double Json::getNumber() const
{
    if (type() != Type::Number)
    {
        auto fmt = "Can't call 'getNumber()' on a %s value: not a Number";
        throw typeError(fmt, type());
    }

    return std::get<double>(value);
}

const std::string& Json::getString() const
{
    if (type() != Type::String)
        throw typeError("Can't get string from %s: not a String", type());

    return std::get<std::string>(value);
}

bool Json::isNull() const noexcept
{
    return type() == Type::Null;
}

size_t Json::size() const
{
    switch (type()) {
    case Type::Null:
    case Type::Bool:
    case Type::Number:
        throw typeError("Can't call 'size()' on a %s value", type());
    case Type::String:
        return std::get<std::string>(value).size();
    case Type::Array:
        return std::get<Array>(value).size();
    case Type::Object:
        return std::get<Object>(value).size();
    }
    throw std::logic_error("Function must return inside switch");
}

std::set<std::string> Json::keys() const
{
    if (type() != Type::Object)
        throw typeError("Can't call 'keys()' on a %s value: not an Object", type());

    std::set<std::string> keys;
    for (const auto& [key, value] : std::get<Object>(value))
        keys.insert(key);
    return keys;
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
    }
    throw std::logic_error("Function must return inside switch");
}

Json::Type Json::type() const noexcept
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
    }
    throw std::logic_error("Function must return earlier");
}

bool operator!=(const Json& left, const Json& right)
{
    return !(left == right);
}

std::istream& operator>>(std::istream& istream, Json& json)
{
    Parser p(istream,
             Json::WhitespaceAfter::Ignore, // both are possible to change in
             Json::DuplicateKeys::Ignore);  // Json::parse(std::istream&), if need to
    json = p.parse();
    return istream;
}

} // end of namespace "expurple"
