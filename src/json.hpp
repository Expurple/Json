#ifndef EXPURPLE_JSON_JSON_HPP
#define EXPURPLE_JSON_JSON_HPP

#include <memory>
#include <set>
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
// types:

    // Parsing options:
    enum class DuplicateKeys : unsigned char
    {
        Ignore,
        Check
    };
    enum class WhitespaceAfter : unsigned char
    {
        Ignore,
        Check
    };

    // Exception types:
    class ParseError : public std::invalid_argument
    {
    public:
        explicit ParseError(const std::string& reason)
            : std::invalid_argument(reason) {}
        explicit ParseError(const std::string& reason, long long index)
            : std::invalid_argument(reason + " at index " + std::to_string(index)) {}
    };

    class TypeError : public std::logic_error
    {
    public:
        explicit TypeError(const std::string& msg) : std::logic_error(msg) {}
    };

    class KeyError : public std::invalid_argument
    {
    public:
        explicit KeyError(const std::string& msg) : std::invalid_argument(msg) {}
    };

    class IndexError : public std::invalid_argument
    {
    public:
        explicit IndexError(const std::string& msg) : std::invalid_argument(msg) {}
    };

// constructors and destructor:
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

// operators:

    Json& operator=(const Json& other);
    Json& operator=(Json&& other);

    friend bool operator==(const Json& left, const Json& right);
    friend bool operator!=(const Json& left, const Json& right);

    Json& operator[](const std::string& key);
    Json& operator[](size_t index);

// methods:

    static Json array();
    static Json parse(const char* str,
                      WhitespaceAfter wsafter = WhitespaceAfter::Check,
                      DuplicateKeys dkeys = DuplicateKeys::Ignore);
    static Json parse(const std::string& str,
                      WhitespaceAfter wsafter = WhitespaceAfter::Check,
                      DuplicateKeys dkeys = DuplicateKeys::Ignore);
    static Json parse(std::istream& istream,
                      WhitespaceAfter wsafter = WhitespaceAfter::Ignore,
                      DuplicateKeys dkeys = DuplicateKeys::Ignore);

    Json& at(const std::string& key);
    Json& at(size_t index);
    void push_back(const Json& val);

    const std::string& getString() const;
    size_t size() const;
    std::set<std::string> keys() const;

private:
// types:

    enum class Type
    {
        Null = 0,
        Bool = 1,
        Number = 2,
        String = 3,
        Array = 4,
        Object = 5
    };

    // All these types CAN'T BE DIRECTLY COPIED because of std::unique_ptr!
    // std::unique_ptr is used because can't store incomplete type Json by value.
    using JsonPtr = std::unique_ptr<Json>;
    using Array = std::vector<JsonPtr>;
    using Object = std::unordered_map<std::string, JsonPtr>;
    using Value = std::variant <std::nullptr_t, bool, double, std::string, Array, Object>;

// methods:

    Json(Array&& val);
    Json(Object&& val);

    static Array copy(const Array& array);
    static Object copy(const Object& object);
    static Json::Value copy(const Json& json);

    Type type() const;

    static bool areEqual(const Array& left, const Array& right);
    static bool areEqual(const Object& left, const Object& right);

// fields:

    Value value;
};

std::istream& operator>>(std::istream& istream, Json& json);

// todo: operator<< overload, operator| overload

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_JSON_HPP
