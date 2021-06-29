#include "doctest.h"

#include "json.hpp"

using namespace expurple;

TEST_CASE("test Json class") {
    SUBCASE("test constructing, moving, copying and comparing") {
        SUBCASE("primitive values should be constructed and compared implicitly") {
            Json a = 5;
            CHECK_EQ(a, 5);

            Json b = true;
            CHECK_NE(b, false);
        }
        SUBCASE("different value types should never be equal") {
            CHECK_NE(Json(1), Json(true));
            CHECK_NE(Json(2), Json("2"));
            CHECK_NE(Json(nullptr), Json(false));
            CHECK_NE(Json(0), Json::array());
            CHECK_NE(Json(), Json::array());
        }
        SUBCASE("move construction/assignment should work as expected") {
            Json originalOwner = Json::parse("[1, 2, 3, 4, 5]");
            Json newOwner = std::move(originalOwner);
            CHECK_EQ(originalOwner, Json::parse("[]"));
            CHECK_EQ(newOwner, Json::parse("[1, 2, 3, 4, 5]"));
        }
    }

    SUBCASE("test access methods") {
        SUBCASE("typed getters should work as expected") {
            CHECK_EQ(Json(false).getBool(), false);
            CHECK_EQ(Json(0.5).getDouble(), 0.5);
            CHECK_EQ(Json("abc").getString(), "abc");

            SUBCASE("and fail when called on a wrong type of Json") {
                CHECK_THROWS_AS(Json(nullptr).getBool(), Json::TypeError);
                CHECK_THROWS_AS(Json(4).getString(), Json::TypeError);
                CHECK_THROWS_AS(Json("abcd").getDouble(), Json::TypeError);
            }
        }

        SUBCASE("Json::at() should fail on out-of-bounds access") {
            CHECK_THROWS_AS(Json().at("non existing key"), Json::KeyError);
            CHECK_THROWS_AS(Json::parse("{\"a\" : \"b\"}").at("c"), Json::KeyError);
            CHECK_THROWS_AS(Json::array().at(0), Json::IndexError);
        }
    }

    SUBCASE("test observer methods") {
        SUBCASE("isNull() should return true for Json constructed from null "
                "and false for others (no type casting)") {
            CHECK(      Json(nullptr).isNull());
            CHECK(Json::parse("null").isNull());

            CHECK_FALSE(  Json(false).isNull());
            CHECK_FALSE(      Json(0).isNull());
            CHECK_FALSE(     Json("").isNull());
            CHECK_FALSE(Json::array().isNull());
            CHECK_FALSE(       Json().isNull());
        }
        SUBCASE("size() should work with String, Array and Object values") {
            CHECK_EQ(Json("abc").size(), 3);
            CHECK_EQ(Json::array().size(), 0);
            CHECK_EQ(Json::parse("{\"1\": 1, \"2\": 2}").size(), 2);

            SUBCASE("and fail with Null, Bool and Number values") {
                CHECK_THROWS_AS(Json(nullptr).size(), Json::TypeError);
                CHECK_THROWS_AS(Json(true).size(),    Json::TypeError);
                CHECK_THROWS_AS(Json(15).size(),      Json::TypeError);
            }
        }
        SUBCASE("keys() should work as expected with Object and fail with other types") {
            auto actual = Json::parse("{\"a\": 1, \"b\": 2}").keys();
            std::set<std::string> expected = {"a", "b"};
            CHECK_EQ(expected, actual);

            CHECK_THROWS_AS(Json(nullptr).keys(), Json::TypeError);
            CHECK_THROWS_AS(Json(true).keys(),    Json::TypeError);
            CHECK_THROWS_AS(Json(15).keys(),      Json::TypeError);
            CHECK_THROWS_AS(Json("ab").keys(),    Json::TypeError);
            CHECK_THROWS_AS(Json::array().keys(), Json::TypeError);
        }
    }

    SUBCASE("test parsing and stringification") {
        SUBCASE("should be able to stringify a Json and parse the same exact value back") {
            // TODO: proper initialization, when there is any
            Json original;
            original["array"] = Json::parse("[1, 2, 3]");
            original["flag"] = false;
            std::string stringDump = original.toString();
            Json copy = Json::parse(stringDump);
            CHECK_EQ(original, copy);
        }
    }
}
