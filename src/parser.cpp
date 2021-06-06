#include "parser.hpp"

#include <cctype>
#include <cstdint>
#include <sstream>
#include <iostream>
#include "util.hpp"

namespace expurple
{

using ParseError = Json::ParseError;


Parser::Parser(std::istream& istream, WhitespaceAfter wsafter, DuplicateKeys dkeys)
    : startIndex((long long)istream.tellg() - 1),
      stream(istream), duplicateKeys(dkeys), whitespaceAfter(wsafter)
{
    if (!istream)
        throw ParseError("istream is in bad state");
}

Json Parser::parse()
{
    skipWhitespace();
    Json json = parseValue();
    if (whitespaceAfter == WhitespaceAfter::Check)
        checkWhitespaceUntilEnd();
    return json;
}

Json Parser::parseValue()
{
    switch(peekChar()) {
    case 'n':
        return parseNull();
    case 't':
    case 'f':
        return parseBool();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return parseNumber();
    case '"':
        return parseString();
    case '[':
        return parseArray();
    case '{':
        return parseObject();
    default:
    {
        auto msg = format("unexpected character '%c'", stream.peek());
        throw ParseError(msg, currentIndex() + 1);
    }}
}

Json Parser::parseNull()
{
    parseLiteral("null");
    return Json(nullptr);
}

Json Parser::parseBool()
{
    char firstChar = parseOneOf('t', 'f');
    if (firstChar == 't')
    {
        parseLiteral("rue");
        return Json(true);
    }
    else
    {
        parseLiteral("alse");
        return Json(false);
    }
}

Json Parser::parseNumber()
{
    // KISS, lol
    long long indexAtNumber = currentIndex() + 1;
    double number;
    if (!(stream >> number))
        throw ParseError("attempted to parse a number and failed", indexAtNumber);
    return Json(number);
}

Json Parser::parseString()
{
    std::string buf;
    parseLiteral('"');
    while (peekChar() != '"')
    {
        if (peekChar() == '\\')
        {
            parseLiteral('\\');
            char ch = getChar();
            if (ch == 'u')
            {
                buf += "\\u";
                buf += parseFourHexDigits();
            }
            else // should be a single escaped char
            {
                char escapedChar = escapeChars.at(escape(ch));
                buf.push_back(escapedChar);
            }
        }
        else // ordinary non escaped char
        {
            buf.push_back(getChar());
        }
    }
    parseLiteral('"');
    return Json(buf.c_str());
}

Json Parser::parseArray()
{
    Json result = Json::array();
    parseLiteral('[');
    skipWhitespace();
    if (peekChar() != ']')
    {
        result.push_back(parseValue());
        skipWhitespace();
    }
    while (parseOneOf(',', ']') == ',')
    {
        skipWhitespace();
        result.push_back(parseValue());
        skipWhitespace();
    }
    return result;
}

Json Parser::parseObject()
{
    Json result;
    parseLiteral('{');
    skipWhitespace();
    if (peekChar() != '}')
    {
        auto keyValue = parseKeyValueWithWsAround(result);
        result[keyValue.first] = keyValue.second;
    }
    while (parseOneOf(',', '}') == ',')
    {
        auto keyValue = parseKeyValueWithWsAround(result);
        result[keyValue.first] = keyValue.second;
    }
    return result;
}

std::pair<std::string, Json> Parser::parseKeyValueWithWsAround(const Json& objectSoFar)
{
    skipWhitespace();
    std::string key = parseString().getString();
    if (duplicateKeys == DuplicateKeys::Check && objectSoFar.keys().count(key) == 1)
        throw ParseError("duplicate key \"" + key + "\"", currentIndex());
    skipWhitespace();
    parseLiteral(':');
    skipWhitespace();
    Json value = parseValue();
    skipWhitespace();
    return {key, value};
}

std::string Parser::parseFourHexDigits()
{
    std::string buf(4, '\0');
    for (auto& ch : buf)
    {
        if (!isxdigit(peekChar()))
            throw ParseError("expected a hexidecimal character", currentIndex() + 1);
        ch = stream.get();
    }
    return buf;
}

void Parser::parseLiteral(const std::string &literal)
{
    for (auto ch : literal)
        parseLiteral(ch);
}

void Parser::parseLiteral(char literal)
{
    char ch = getChar();
    if (ch != literal)
    {
        auto msg = format("expected '%c', but got '%c'", literal, ch);
        throw ParseError(msg, currentIndex());
    }
}

char Parser::parseOneOf(char first, char second)
{
    char ch = getChar();
    if (ch != first && ch != second)
    {
        auto msg = format("expected '%с' or '%с', but got '%с'", first, second, ch);
        throw ParseError(msg, currentIndex());
    }
    return ch;
}

void Parser::skipWhitespace()
{
    while (true)
    {
        checkForBadChar();
        if (!isspace(stream.peek()))
            break;
        stream.get();
    }
}

void Parser::checkWhitespaceUntilEnd()
{
    if (stream.eof())
        return;

    for (char ch = stream.peek(); ch != '\0' && ch != EOF; ch = stream.get())
    {
        if (!isspace(ch))
        {
            auto msgFormat = "found unexpected non-whitespace character"
                             " '%c' (char code %u) after Json end";
            auto msg = format(msgFormat, ch, (unsigned char)ch);
            throw ParseError(msg, currentIndex() + 1);
        }
    }
}

char Parser::escape(char ch)
{
    try {
        return escapeChars.at(ch);
    }
    catch (const std::out_of_range&) {
        auto msg = format("invalid escape sequence '\\%c'", ch);
        throw ParseError(msg, currentIndex());
    }
}

char Parser::getChar()
{
    checkForBadChar();
    return stream.get();
}

char Parser::peekChar()
{
    checkForBadChar();
    return stream.peek();
}

void Parser::checkForBadChar()
{
    char ch = stream.peek();
    if (ch == '\0')
        throw ParseError("unexpected null-terminator", currentIndex() + 1);
    if (ch == EOF)
        throw ParseError("unexpected EOF", currentIndex() + 1);
    if (ch < 0x0020 && !isspace(ch))
    {
        auto charCode = (unsigned char)ch;
        auto message = format("unexpected special character (char code %u)", charCode);
        throw Json::ParseError(message, currentIndex() + 1);
    }
}

const std::unordered_map<char, char> Parser::escapeChars = {
    {'\\', '\\'},
    {'"', '"'},
    {'b', '\b'},
    {'f', '\f'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'}
};

} // end of namespace "expurple"
