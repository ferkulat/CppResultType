#include <doctest/doctest.h>

#include <result_type/helper/stream.hpp>
#include <result_type/result.hpp>
#include <result_type/pipeoperator.hpp>

#include <sstream>
using result_type::operator|;
enum class Error{lol};
std::ostream& operator<<(std::ostream& os, Error const& ){
    os << "lol";
    return os;
}

TEST_CASE("Passing Result<Success, Error>(Success())"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<std::string,Error>("hi there :)") | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Result<std::string,Error>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "hi there :)");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<std::string,Error>("hi there :)") | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Result<std::string,Error>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing Result<Optional<Success>, Error>(Optional<Success>(Success))"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<result_type::Optional<std::string>,Error>("hi there :)") | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Result<result_type::Optional<std::string>,Error>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "hi there :)");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<result_type::Optional<std::string>,Error>("hi there :)") | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Result<result_type::Optional<std::string>,Error>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing Result<Optional<Success>, Error>(Optional<Success>())"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<result_type::Optional<std::string>,Error>(result_type::Optional<std::string>()) | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Result<result_type::Optional<std::string>,Error>(result_type::Optional<std::string>());

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<result_type::Optional<std::string>,Error>(result_type::Optional<std::string>()) | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Result<result_type::Optional<std::string>,Error>(result_type::Optional<std::string>());

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing Optional<T>(Optional<T>(T()))"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Optional<std::string>("hi there :)") | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Optional<std::string>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "hi there :)");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Optional<std::string>("hi there :)") | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Optional<std::string>("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing Optional<T>()"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Optional<std::string>() | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Optional<std::string>();

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Optional<std::string>() | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Optional<std::string>();

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing Result<Success, Error>(Error())"){
    SUBCASE(" to streamErrorTo(an_output_stream), streams the Error value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<std::string,Error>(Error{}) | result_type::helper::streamErrorTo(output_stream);
        auto const expected = result_type::Result<std::string,Error>(Error{});

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "lol");
    }
    SUBCASE(" to streamSuccessTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = result_type::Result<std::string,Error>(Error{}) | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = result_type::Result<std::string,Error>(Error{});

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}

TEST_CASE("Passing T, that is not an Optional and not a Result<S,E>"){
    SUBCASE(" to streamSuccessTo(an_output_stream), streams the success value and returns the input value"){
        auto output_stream  = std::stringstream{};
        auto const actual   = std::string("hi there :)") | result_type::helper::streamSuccessTo(output_stream);
        auto const expected = std::string("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "hi there :)");
    }
    SUBCASE(" to streamErrorTo(an_output_stream), but does not stream anything"){
        auto output_stream  = std::stringstream{};
        auto const actual   = std::string("hi there :)") | result_type::helper::streamErrorTo(output_stream);
        auto const expected = std::string("hi there :)");

        REQUIRE(std::is_same<decltype(actual), decltype(expected)>::value);
        REQUIRE(actual == expected);
        REQUIRE(output_stream.str() == "");
    }
}
