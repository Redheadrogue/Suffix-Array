#include <divsufsort.h>
#include <sstream>

#include <algorithm>
#include <chrono>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>

// Aggregate results into a volatile sink so the compiler cannot optimise away
// the search loops in Release builds.
static volatile uint64_t g_total_hits = 0;

// Compare the prefix of suffix reference[pos..] with query.
// Returns:
//   <0 if suffix_prefix < query
//    0 if query fully matches as prefix (=> occurrence)
//   >0 if suffix_prefix > query
static inline int compare_suffix_query(std::vector<seqan3::dna5> const & reference,
                                       saidx_t pos,
                                       std::vector<seqan3::dna5> const & query)
{
    size_t const n = reference.size();
    size_t const m = query.size();

    for (size_t j = 0; j < m; ++j)
    {
        size_t const i = static_cast<size_t>(pos) + j;
        if (i >= n)
            return -1; // suffix ended early

        auto const a = seqan3::to_rank(reference[i]);
        auto const b = seqan3::to_rank(query[j]);
        if (a < b) return -1;
        if (a > b) return 1;
    }
    return 0; // query matches as prefix
}

int main(int argc, char const * const * argv)
{
    seqan3::argument_parser parser{"suffixarray_search", argc, argv, seqan3::update_notifications::off};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    auto reference_file = std::filesystem::path{};
    parser.add_option(reference_file, '\0', "reference", "path to the reference file");

    auto query_file = std::filesystem::path{};
    parser.add_option(query_file, '\0', "query", "path to the query file");

    auto number_of_queries = size_t{100};
    parser.add_option(number_of_queries, '\0', "query_ct",
                      "number of query, if not enough queries, these will be duplicated");

    try
    {
        parser.parse();
    }
    catch (seqan3::argument_parser_error const & ext)
    {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto reference_stream = seqan3::sequence_file_input{reference_file};
    auto query_stream     = seqan3::sequence_file_input{query_file};

    // Concatenate reference sequences into one big vector.
    std::vector<seqan3::dna5> reference;
    for (auto & record : reference_stream)
    {
        auto r = record.sequence();
        reference.insert(reference.end(), r.begin(), r.end());
    }

    std::vector<std::vector<seqan3::dna5>> queries;
    for (auto & record : query_stream)
        queries.push_back(record.sequence());

    while (queries.size() < number_of_queries)
    {
        auto old_count = queries.size();
        queries.resize(2 * old_count);
        std::copy_n(queries.begin(), old_count, queries.begin() + old_count);
    }
    queries.resize(number_of_queries);

    std::vector<saidx_t> suffixarray(reference.size());

    // -----------------------------
    // 1) Build suffix array (index)
    // -----------------------------
    auto const build_t0 = std::chrono::steady_clock::now();

    sauchar_t const * str = reinterpret_cast<sauchar_t const *>(reference.data());
    int const rc = divsufsort(str, suffixarray.data(), static_cast<saidx_t>(reference.size()));
    if (rc != 0)
    {
        seqan3::debug_stream << "divsufsort failed with code " << rc << "\n";
        return EXIT_FAILURE;
    }

    auto const build_t1 = std::chrono::steady_clock::now();

    // -----------------------------
    // 2) Query using binary search
    // -----------------------------
    auto const search_t0 = std::chrono::steady_clock::now();

    for (auto const & q : queries)
    {
        if (q.empty())
            continue;

        auto lb = std::lower_bound(
            suffixarray.begin(), suffixarray.end(), q,
            [&](saidx_t const pos, std::vector<seqan3::dna5> const & query)
            {
                return compare_suffix_query(reference, pos, query) < 0;
            });

        auto ub = std::upper_bound(
            suffixarray.begin(), suffixarray.end(), q,
            [&](std::vector<seqan3::dna5> const & query, saidx_t const pos)
            {
                return compare_suffix_query(reference, pos, query) > 0;
            });

        g_total_hits += static_cast<uint64_t>(std::distance(lb, ub));
    }

    auto const search_t1 = std::chrono::steady_clock::now();

    auto const build_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(build_t1 - build_t0).count();
    auto const search_ms = std::chrono::duration_cast<std::chrono::milliseconds>(search_t1 - search_t0).count();

    seqan3::debug_stream << "total_hits\t" << g_total_hits << "\n";
    seqan3::debug_stream << "build_ms\t"  << build_ms << "\n";
    seqan3::debug_stream << "search_ms\t" << search_ms << "\n";
    return 0;
}
