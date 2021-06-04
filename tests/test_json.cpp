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
        }
#ifdef EXPURPLE_JSON_DEBUG_MOVE
        SUBCASE("assignment or comparison of moved-from Jsons should fail in debug") {
            Json moved = "some value to pass";
            Json alive = std::move(moved);
            CHECK_THROWS_AS(alive == moved,           Json::MoveError);
            CHECK_THROWS_AS(alive = moved,            Json::MoveError);
            CHECK_THROWS_AS(alive = std::move(moved), Json::MoveError);
        }
#endif
    }
}
