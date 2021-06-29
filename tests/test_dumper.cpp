#include "doctest.h"

#include "dumper.hpp"

using namespace expurple;

TEST_CASE("test json Dumper") {
    SUBCASE("strings should be escaped properly") {
        const Json json = "line 1"      "\n"
                          "\"line 2 in quotes\"";
        std::string expected = "\\\"line 1\\n\\\"line 2 in quotes\\\"";
        std::string actual = json.toString();
        CHECK_EQ(expected, actual);
    }
    SUBCASE("there should be no comma after the last element") {
        const auto json = Json::parse("[1, 2, 3]");
        std::string expected = "[1,2,3]";
        std::string actual = json.toString();
        CHECK_EQ(expected, actual);
    }
    SUBCASE("keys should always be in lexicographical order") {
        const auto json = Json::parse(R"({"b":null,"c":null,"a":null})");
        std::string expected = R"({"a":null,"b":null,"c":null})";
        std::string actual = json.toString(Json::Whitespace::None);
        CHECK_EQ(expected, actual);
    }
    SUBCASE("whitespace options should behave like this") {
        const auto json = Json::parse(R"({"a": 1, "b": 2})");
        std::string actual, expected;

        actual = json.toString(Json::Whitespace::None);
        expected = R"({"a":1,"b":2})";
        CHECK_EQ(expected, actual);

        actual = json.toString(Json::Whitespace::Space);
        expected = R"({"a": 1, "b": 2})";
        CHECK_EQ(expected, actual);

        actual = json.toString(Json::Whitespace::NewlineAndTab);
        expected = "{"                 "\n"
                   "\t"  R"("a": 1,)"  "\n"
                   "\t"  R"("b": 2)"   "\n"
                   "}";
        CHECK_EQ(expected, actual);
    }
}
