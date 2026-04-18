#include <format>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <vdf_parser.hpp>

#include <benchmark/benchmark.h>

struct VdfGeneratorParams
{
    size_t attributes = 0;
    size_t wordSize = 0;
    size_t maxDepth = 0;
    size_t vdfObjects = 0;
};

struct VdfGeneratorState
{
    size_t depth = 0;
    size_t index = 0;

    struct Rng
    {
        std::mt19937 generator{1234}; // std::random_device{}()
    };

    std::shared_ptr<Rng> rng = std::make_shared<Rng>();
};

std::string generate_random_string(size_t length,
                                   VdfGeneratorState const &state)
{
    constexpr std::string_view characters =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::uniform_int_distribution<> distribution(0, characters.length() - 1);

    std::string random_string;
    random_string.reserve(length);
    for (size_t i = 0; i < length; ++i)
    {
        random_string += characters[distribution(state.rng->generator)];
    }
    return random_string;
}

std::string generate_vdf_structure(VdfGeneratorParams const &params,
                                   VdfGeneratorState const &state)
{
    std::stringstream item;
    item << std::format("\"vdf_object_{}_{}\"{{", state.depth, state.index);
    for (size_t i = 0; i < params.attributes; ++i)
    {
        item << std::format("\"item_{}\" \"{}\"", i,
                            generate_random_string(params.wordSize, state));
    }

    if (state.depth < params.maxDepth)
    {
        for (size_t i = 0; i < params.maxDepth; ++i)
        {
            item << generate_vdf_structure(
                params, VdfGeneratorState{.depth = state.depth + 1,
                                          .index = i,
                                          .rng = state.rng});
        }
    }

    item << "}";
    return item.str();
}

std::string generate_vdf_structure(VdfGeneratorParams const &params)
{
    return generate_vdf_structure(params, VdfGeneratorState{});
}

static void BM_ReadGeneratedVDFObject(benchmark::State &state)
{
    auto vdfString = generate_vdf_structure(VdfGeneratorParams{
        .attributes = 20, .wordSize = 10, .maxDepth = 5, .vdfObjects = 3});

    for (auto _ : state)
    {
        std::ignore = tyti::vdf::read(vdfString.begin(), vdfString.end());
    }
}

// Register the benchmark
BENCHMARK(BM_ReadGeneratedVDFObject)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5'000);

BENCHMARK_MAIN();