#ifndef EXPURPLE_JSON_PARSER_HPP
#define EXPURPLE_JSON_PARSER_HPP

#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

#include "json.hpp"

namespace expurple
{

// A temporary object for each call of Json::parse() or Json::operator>>,
// holding internal parsing state and doing all the job.
class Parser
{
public:
    using WhitespaceAfter = Json::WhitespaceAfter;
    using DuplicateKeys = Json::DuplicateKeys;

    explicit Parser(std::istream& istream, WhitespaceAfter wsafter, DuplicateKeys dkeys);
    Json parse();

private:
    // parse any value type
    Json parseValue();
    // parse some particular value type:
    Json parseNull();
    Json parseBool();
    Json parseNumber();
    Json parseString();
    Json parseArray();
    Json parseObject();
    // helpers:
    std::pair<std::string, Json> parseKeyValueWithWsAround(const Json& objectSoFar);
    std::string parseFourHexDigits();
    void parseLiteral(const std::string& literal);
    void parseLiteral(char literal);
    char parseOneOf(char first, char second);
    void skipWhitespace();
    void checkWhitespaceUntilEnd();
    char escape(char ch);
    char getChar();
    char peekChar();
    void checkForBadChar();

    inline long long currentIndex()
    {
        return static_cast<long long>(stream.tellg()) - startIndex;
    }

// fields:

    long long startIndex;
    std::istream& stream;
    const DuplicateKeys duplicateKeys;
    const WhitespaceAfter whitespaceAfter;
    static const std::unordered_map<char, char> escapeChars;
};

} // end of namespace "expurple"

#endif // EXPURPLE_JSON_PARSER_HPP
