#include "doctest.h"

#include <fstream>

#include "parser.hpp"

using namespace expurple;

// general behaviour tests:

TEST_CASE("Parser constructor should throw if istream is in bad state") {
    std::stringstream ss("");
    ss.get();  // trigger EOF state by reading past the end of the stream
    CHECK_THROWS_AS(Parser(ss, Json::WhitespaceAfter::Ignore,
                           Json::DuplicateKeys::Ignore),
                    Json::ParseError);
}

TEST_CASE("test correct Json value parsing") {
    SUBCASE("test Null value") {
        SUBCASE("Json::parse() should create same Json as constructor") {
            Json constructed(nullptr);
            Json parsed = Json::parse("null");
            CHECK_EQ(constructed, parsed);
        }
    }
    SUBCASE("test Bool value") {
        SUBCASE("Json::parse() should create same Json as constructor") {
            Json constructed(true);
            Json parsed = Json::parse("true");
            CHECK_EQ(constructed, parsed);
        }
    }
    SUBCASE("test Int value") {
        SUBCASE("Json::parse() should create same Json as constructor") {
            Json constructed = 3;
            Json parsed = Json::parse("3");
            CHECK_EQ(constructed, parsed);
        }
    }
    SUBCASE("test Double value") {
        SUBCASE("Json::parse() should create same Json as constructor") {
            Json constructed = 0.2;
            Json parsed = Json::parse("0.2");
            CHECK_EQ(constructed, parsed);
        }
    }
    SUBCASE("test String value") {
        SUBCASE("Json::parse() should create same Json as constructor (minus quotes)") {
            Json constructed("value");
            Json parsed = Json::parse("\"value\"");
            CHECK_EQ(constructed, parsed);
        }
        SUBCASE("escaped 2 byte Unicode code points ('\\uFFFF', where F are hex digits)"
                " are left in text as is") {
            // "\\u014F" == u8"ŏ", so this string is basically an encoded u8"hellŏ"
            Json constructed("hell\\u014F");
            Json parsed = Json::parse("\"hell\\u014F\"");
            CHECK_EQ(constructed, parsed);

            SUBCASE("in practice, longer hex sequences also remain as is") {
                // there's no way to tell if this is actually + "\\u0123" + "45",
                // but it doesn't matter
                Json constructed("\\u012345");
                Json parsed = Json::parse("\"\\u012345\"");
                CHECK_EQ(constructed, parsed);
            }
        }
    }
    SUBCASE("test Array value") {
        SUBCASE("Json::parse() should create same Json as regular assingment") {
            Json constructed = Json::parse("[]");
            constructed.push_back(1);
            constructed.push_back("two");
            Json parsed = Json::parse("[1, \"two\"]");
            CHECK_EQ(constructed, parsed);
        }
    }
    SUBCASE("test Object value") {
        SUBCASE("Json::parse() should create same Json as assignment by key") {
            Json constructed;
            constructed["key"] = "value";
            Json parsed = Json::parse("{ \"key\" : \"value\" }");
            CHECK_EQ(constructed, parsed);
        }
    }
}

TEST_CASE("test errors on invalid json strings") {
    SUBCASE("these 'bad numbers' should fail") {
        CHECK_THROWS_AS(Json::parse("5-"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("1E+boi"), Json::ParseError);

        SUBCASE("must not start with '.'") {
            CHECK_THROWS_AS(Json::parse(".67"), Json::ParseError);
        }
        SUBCASE("also, must not start with '+'") {
            CHECK_THROWS_AS(Json::parse("+0"), Json::ParseError);
            CHECK_THROWS_AS(Json::parse("+3.4"), Json::ParseError);
        }
    }
    SUBCASE ("'\\u' sequences with less than 4 hex digits should fail") {
        CHECK_THROWS_AS(Json::parse("\"\\usa\""), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("\"\\uAATA\""), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("\"\\u00\\u1122\""), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("\"\\u012\""), Json::ParseError);
    }
    SUBCASE("unexpected EOF should fail") {
        CHECK_THROWS_AS(Json::parse("nu"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("fals"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("\"hello worl"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("[ 1, "), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("[2"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("{ \"hello\" : \"world\" "), Json::ParseError);
    }
    SUBCASE("non-string Object keys should fail") {
        CHECK_THROWS_AS(Json::parse("{ 1 : \"2\" }"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("{"
                                    "  { \"a whole\" : \"object key\" } : \"value\""
                                    "}"), Json::ParseError);
        CHECK_THROWS_AS(Json::parse("{ [\"k\", \"e\", \"y\"] : \"value\" }"),
                        Json::ParseError);
    }
}

auto jsonWithDuplicateKeys = "{\"duplicateKey\": \"firstValue\","
                             " \"duplicateKey\": \"lastValue\"}";
TEST_CASE("with duplicate keys, last value should be silently used by default") {
    Json json;
    CHECK_NOTHROW(json = Json::parse(jsonWithDuplicateKeys));
    CHECK_EQ(json["duplicateKey"], "lastValue");

    SUBCASE("but this can be changed by setting 'dkeys' to 'Check'") {
        CHECK_THROWS_AS(Json::parse(jsonWithDuplicateKeys,
                                    Json::WhitespaceAfter::Check,
                                    Json::DuplicateKeys::Check),
                        Json::ParseError);
    }
}



// specific tests for each function/operator:


// Invalid escape sequence '\?'
// would throw Json::ParseError() in Json::parse()
// if not explicitly ignoring string end
const char* stringWithBadEnd = "{\"key\": \"value\"}\\?";

TEST_CASE("test operator>> specifically") {
    SUBCASE("should not read past the end of Json") {
        std::stringstream ss(stringWithBadEnd);
        Json json;
        CHECK_NOTHROW(ss >> json);
        CHECK_EQ(json["key"], "value");

        SUBCASE("the same string should fail when parsed as a whole") {
            CHECK_THROWS_AS(Json::parse(stringWithBadEnd), Json::ParseError);
        }
    }
}

TEST_CASE("test Json::parse(const char*) and Json::parse(const std::string&)"
          " difference from operator>>") {
    SUBCASE("parse functions should fail on extra non-whitespace characters at the end") {
        const char* jsonStr = "{\"key\": \"value\"} extra stuff";
        CHECK_THROWS_AS(Json::parse(jsonStr), Json::ParseError);

        SUBCASE("but this can be disabled by setting 'wsafter' to 'Ignore'") {
            const char* jsonStr = "{\"key\": \"value\"} extra stuff";
            Json json;
            CHECK_NOTHROW(json = Json::parse(jsonStr, Json::WhitespaceAfter::Ignore));
            CHECK_EQ(json["key"], "value");
        }

        SUBCASE("whitespace characters should pass by default") {
            const char* jsonStr = "{\"key\": \"value\"}  \t  \n  ";
            Json json;
            CHECK_NOTHROW(json = Json::parse(jsonStr));
            CHECK_EQ(json["key"], "value");
        }
    }
}

TEST_CASE("test Json::parse(std::istream&) specifics") {
    SUBCASE("should not read past the end of Json by default, same as operator>>") {
        std::stringstream ss(stringWithBadEnd);
        Json json;
        CHECK_NOTHROW(json = Json::parse(ss));
        CHECK_EQ(json["key"], "value");

        SUBCASE("but this can be inverted, same as with string functions") {
            std::stringstream ss(stringWithBadEnd);
            CHECK_THROWS_AS(Json::parse(ss, Json::WhitespaceAfter::Check),
                            Json::ParseError);
        }
    }
}
