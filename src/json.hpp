#ifndef EXPURPLE_JSON_JSON_HPP
#define EXPURPLE_JSON_JSON_HPP

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <unordered_map>

namespace expurple {

class Json
{
public:
    Json();
    Json(const Json& other);
    Json(Json&& other);
    Json(std::nullptr_t) noexcept;
    Json(bool val) noexcept;
    Json(const char* val);
    Json(const std::string& val);

    template<typename NumericT>
    Json(NumericT val) noexcept
        : value(static_cast<double>(val))
    {
        static_assert(std::is_arithmetic<NumericT>::value,
                      "This overload is for casting any numbers into double");
    };

    ~Json() noexcept = default;

    Json& operator=(const Json& other);
    Json& operator=(Json&& other);

    friend bool operator==(const Json& left, const Json& right);
    friend bool operator!=(const Json& left, const Json& right);

#ifdef EXPURPLE_JSON_DEBUG_MOVE
    class MoveError : public std::logic_error
    {
    public:
        explicit MoveError(const char* msg) : std::logic_error(msg) {}
    };
#endif
private:
// types:

    enum class Type
    {
        Null = 0,
        Bool = 1,
        Number = 2,
        String = 3,
        Array = 4,
        Object = 5,
        Moved = 6 // special invalid state after being moved-from
    };

    struct Moved // a placeholder type for std::variant
    {
        constexpr bool operator==(const Moved) const noexcept { return true; }
    };

    // All these types CAN'T BE DIRECTLY COPIED because of std::unique_ptr!
    // std::unique_ptr is used because can't store incomplete type Json by value.
    using JsonPtr = std::unique_ptr<Json>;
    using Array = std::vector<JsonPtr>;
    using Object = std::unordered_map<std::string, JsonPtr>;
    using Value = std::variant <std::nullptr_t, bool, double,
                               std::string, Array, Object, Moved>;

// functions:

    Json(Array&& val);
    Json(Object&& val);

    Array copy(const Array& array);
    Object copy(const Object& object);
    Json::Value copy(const Json& json);

    Type type() const;

    static bool areEqual(const Array& left, const Array& right);
    static bool areEqual(const Object& left, const Object& right);

// fields:

    Value value;
};

// todo: iostream operator overload, "|" operator overload

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_JSON_HPP
