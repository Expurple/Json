#ifndef EXPURPLE_JSON_JSON_HPP
#define EXPURPLE_JSON_JSON_HPP

#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace expurple {

class Json
{
public:
// types:

    // Possible value types, as returned by type()
    enum class Type
    {
        Null = 0,
        Bool = 1,
        Number = 2,
        String = 3,
        Array = 4,
        Object = 5
    };

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

    // Dump options:
    enum class Whitespace : unsigned char
    {
        None,
        Space,
        NewlineAndTab
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
        explicit TypeError(const char* fmt, Type thisType); // defined in json.cpp
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
    Json(std::string&& val) noexcept;

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

    const Json& operator[](size_t index) const;

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

    const Json& at(const std::string& key) const;
    const Json& at(size_t index) const;

    bool getBool() const;
    double getNumber() const;
    const std::string& getString() const;

    Type type() const noexcept;
    bool isNull() const noexcept;

    size_t size() const;
    std::set<std::string> keys() const;

    std::string toString(Whitespace ws = Whitespace::None) const;
    void writeTo(std::ostream& ostream, Whitespace ws = Whitespace::None) const;

private:
// types:

    // All these types CAN'T BE DIRECTLY COPIED because of std::unique_ptr!
    // std::unique_ptr is used because can't store incomplete type Json by value.
    using JsonPtr = std::unique_ptr<Json>;
    using Array = std::vector<JsonPtr>;
    using Object = std::map<std::string, JsonPtr>;
    using Value = std::variant <std::nullptr_t, bool, double, std::string, Array, Object>;

// methods:

    Json(Array&& val) noexcept;
    Json(Object&& val);

    static Array copy(const Array& array);
    static Object copy(const Object& object);
    static Value copy(const Json& json);

    static bool areEqual(const Array& left, const Array& right);
    static bool areEqual(const Object& left, const Object& right);

// fields:

    Value value;
};

std::istream& operator>>(std::istream& istream, Json& json);
std::ostream& operator<<(std::ostream& ostream, const Json& json);

// todo: operator| overload

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_JSON_HPP
