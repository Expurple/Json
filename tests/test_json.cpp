#include "doctest.h"

#include "json.hpp"

using namespace expurple;

TEST_CASE("test Json class") {
    SUBCASE("test primitive values") {
        SUBCASE("should be implicitly constructible, copyable and comparable") {
            Json a = 5;
            Json b = a;
            CHECK_EQ(a, b);

            Json c = true;
            CHECK_NE(c, false);
        }
        SUBCASE("different types of Json should never be equal") {
            CHECK_NE(Json(1), Json(true));
            CHECK_NE(Json(2), Json("2"));
            CHECK_NE(Json(nullptr), Json(false));
            CHECK_NE(Json(0), Json::array());
            CHECK_NE(Json(), Json::array());
        }
    }
    SUBCASE("Json::at() should fail on out-of-bounds access") {
        CHECK_THROWS_AS(Json().at("non existing key"), Json::KeyError);
        CHECK_THROWS_AS(Json::parse("{\"a\" : \"b\"}").at("c"), Json::KeyError);
        CHECK_THROWS_AS(Json::array().at(0), Json::IndexError);
    }
}
