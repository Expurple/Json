# Expurple Json

An attempt to write an [nlohmann::json](https://github.com/nlohmann/json)-style library from scratch, using C++17.

NOTE: **This is an educational project, it's not intended for production use**.

It probably has bugs with encodings, escape characters, etc.

## Features:

* parsing from strings and `std::istream`s
    * optional errors on duplicate keys
* stringification (compact or pretty)

## How to build

* `mkdir build && cd build/`
* `cmake ..`
* `cmake --build .`
* now `build/src/` should contain `libexpurple-json.so` (`expurple-json.dll` on Windows)

## How to build and run tests

* build with `EXPURPLE_JSON_BUILD_TESTING` set to `ON`
* run `build/tests/run-tests`

## Licence

This project is licensed under GPL-3.0, see the [LICENSE](LICENSE) file for details.
