#include "json.hpp"

#include <assert.h>

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
    if (this->type() != Type::Object)
        // TODO: better message with exact type
        throw TypeError("cannot index this with string: not an Object");

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
    if (this->type() != Type::Array)
        // TODO: better message with exact type
        throw TypeError("cannot index this with a number: not an Array");

    size_t size = std::get<Array>(value).size();
    if (index >= size)
        throw IndexError("index " + std::to_string(index) + " is out of bounds"
                         " (size is " + std::to_string(size) + ")");

    return *std::get<Array>(value)[index];
}

Json Json::array()
{
    return Array();
}

Json Json::parse(const char* str, WhitespaceAfter wsafter, DuplicateKeys dkeys)
{
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
        throw TypeError("Can't call 'at()' on a Null value");
    case Type::Bool:
        throw TypeError("Can't call 'at()' on a Bool value");
    case Type::Number:
        throw TypeError("Can't call 'at()' on a Number value");
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
            throw KeyError("tried to access a non-existing property '"+ key + "'"); }
    }
    throw std::logic_error("Function must return inside switch");
}

Json& Json::at(size_t index)
{
    switch (type()) {
    case Type::Null:
        throw TypeError("Can't call 'at()' on a Null value");
    case Type::Bool:
        throw TypeError("Can't call 'at()' on a Bool value");
    case Type::Number:
        throw TypeError("Can't call 'at()' on a Number value");
    case Type::String:
        throw TypeError("Can't call 'at()' on a String value."
                        " Try to use 'json.getString().at()'");
    case Type::Array:
        try {
            return *std::get<Array>(value).at(index); }
        catch (const std::out_of_range&) {
            auto fmt = "tried to access value at index %u (size of Array is %u)";
            throw IndexError(format(fmt, index, std::get<Array>(value).size())); }
    case Type::Object:
        throw TypeError("Can't call 'at(size_t)' on an Object value."
                        " Try to use 'at(const std::string&)'");
    }
    throw std::logic_error("Function must return inside switch");
}

void Json::push_back(const Json& val)
{
    switch (type()) {
    case Type::Null:
        throw TypeError("Can't call 'push_back()' on a Null value");
    case Type::Bool:
        throw TypeError("Can't call 'push_back()' on a Bool value");
    case Type::Number:
        throw TypeError("Can't call 'push_back()' on a Number value");
    case Type::String:
        throw TypeError("Can't call 'push_back()' on a String value");
    case Type::Array:
        std::get<Array>(value).emplace_back(new Json(val));
        return;
    case Type::Object:
        throw TypeError("Can't call 'push_back()' on an Object value");
    }
    throw std::logic_error("Function must return inside switch");
}

const std::string& Json::getString() const
{
    if (this->type() != Type::String)
        // TODO: better message with exact type
        throw TypeError("cannot get string from this: not a String");

    return std::get<std::string>(value);
}

size_t Json::size() const
{
    switch (type()) {
    case Type::Null:
        throw TypeError("Can't call 'size()' on a Null value");
    case Type::Bool:
        throw TypeError("Can't call 'size()' on a Bool value");
    case Type::Number:
        throw TypeError("Can't call 'size()' on a Number value");
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
    switch (type()) {
    case Type::Null:
        throw TypeError("Can't call 'keys()' on a Null value");
    case Type::Bool:
        throw TypeError("Can't call 'keys()' on a Bool value");
    case Type::Number:
        throw TypeError("Can't call 'keys()' on a Number value");
    case Type::String:
        throw TypeError("Can't call 'keys()' on a String value");
    case Type::Array:
        throw TypeError("Can't call 'keys()' on an Array value");
    case Type::Object: {
        std::set<std::string> keys;
        for (const auto& [key, value] : std::get<Object>(value))
            keys.insert(key);
        return keys; }
    }
    throw std::logic_error("Function must return inside switch");
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
