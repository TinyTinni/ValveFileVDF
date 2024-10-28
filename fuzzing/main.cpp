#include <cstdint>
#include <string_view>

#include <iostream>
#include <vdf_parser.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    std::string_view test_corpus{reinterpret_cast<const char *>(data), size};
    tyti::vdf::Options opts;
    bool ok;
    auto result =
        tyti::vdf::read(test_corpus.begin(), test_corpus.end(), &ok, opts);
    return 0;
}