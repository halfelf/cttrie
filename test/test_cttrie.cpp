#include <catch2/catch.hpp>
#include <cttrie.h>
#include <string>

void test_trie_case_macro(const char* raw, std::string& s) {
    TRIE(raw)
        s = raw;
    CASE("Rosten")
        s = "Rosten!";
    CASE("Raben")
        s = "Raben@";
    CASE("Rasen")
        s = "Rasen#";
    CASE("Rasten")
        s = "Rasten$";
    CASE("Raster")
        s = "Raster%";
    CASE("Rastender")
        s = "Rastender^";
    CASE("Raban")
        s = "Raban&";
    ENDTRIE;
}

TEST_CASE("Test TRIE/CASE macro", "") {
    std::string target;
    test_trie_case_macro("abc", target);
    REQUIRE(target == "abc");

    test_trie_case_macro("Rastender", target);
    REQUIRE(target == "Rastender^");
}
