#include <cstdint>
#include <string_view>

#include "../vdf_parser.hpp"
#include <iostream>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{

    std::string_view test_corpus{reinterpret_cast<const char *>(data), size};
    bool ok;
    tyti::vdf::Options opt;
    opt.ignore_includes = true;
    auto blub =
        tyti::vdf::read(test_corpus.begin(), test_corpus.end(), &ok, opt);
    return 0;
}