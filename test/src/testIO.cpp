#include <catch2/catch.hpp>
#include <commandline/IO/IOData.h>
#include <string>

static io::IOData generateSampleData() {
    constexpr size_t num_taxa = 8;
    io::IOData data;
    data.mean_rf_dst = 12.3;
    data.git_revision = "GIT_REVISION";
    for (size_t taxon_row = num_taxa; taxon_row > 0; --taxon_row) {
        auto &row = data.pairwise_distance_mtx.emplace_back();
        for (size_t i = 0; i < taxon_row; ++i) {
            row.emplace_back(taxon_row * 10 + i);
        }
    }
    data.split_score_calc = io::IOData::SPI;
    for (size_t taxon = 0; taxon < num_taxa; ++taxon) {
        data.taxa_names.emplace_back(1, static_cast<char>('A' + taxon));
    }
    return data;
}
TEST_CASE("IO serialization and deserialization", "[io]") {
    io::IOData data = generateSampleData();
    nlohmann::json jsonOut;
    io::IOData reconstructed;
    io::to_json(jsonOut, data);
    // can import own json
    std::string json_str = jsonOut.dump(4);
    nlohmann::json jsonIn = nlohmann::json::parse(json_str);
    CHECK_NOTHROW(io::from_json(jsonIn, reconstructed));

    CHECK(reconstructed == data);
    // metrics not stored as number
    nlohmann::json json_metric = io::IOData::SPI;
    std::string metric_str = to_string(json_metric);
    CHECK("\"SPI\"" == metric_str);
}

TEST_CASE("Parse RAXML", "[io]") {
    using namespace io;
    IOData data;
    CHECK(IOData::parse_raxml(
        "../test/samples/reference_results/125/RAxML_info.0",
        "../test/samples/reference_results/125/RAxML_RF-Distances.0",
        data));
}