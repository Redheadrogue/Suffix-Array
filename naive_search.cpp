#include <sstream>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>

#include <chrono>

// Aggregate results into a volatile sink so the compiler cannot optimise away
// the search loops in Release builds.
static volatile uint64_t g_total_hits = 0;

// counts all occurrences of query inside ref
void findOccurences(std::vector<seqan3::dna5> const & ref,
                    std::vector<seqan3::dna5> const & query)
{
    if (query.empty() || ref.size() < query.size())
        return;

    uint64_t hits = 0;
    size_t const last_start = ref.size() - query.size();

    // Naive O(n*m): try every start position and compare the whole pattern.
    for (size_t i = 0; i <= last_start; ++i)
    {
        bool match = true;
        for (size_t j = 0; j < query.size(); ++j)
        {
            if (ref[i + j] != query[j])
            {
                match = false;
                break;
            }
        }
        if (match)
            ++hits;
    }

    g_total_hits += hits;
}

int main(int argc, char const * const * argv)
{
    seqan3::argument_parser parser{"naive_search", argc, argv, seqan3::update_notifications::off};

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

    std::vector<std::vector<seqan3::dna5>> reference;
    for (auto & record : reference_stream)
        reference.push_back(record.sequence());

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

    auto const t0 = std::chrono::steady_clock::now();

    for (auto & r : reference)
        for (auto & q : queries)
            findOccurences(r, q);

    auto const t1 = std::chrono::steady_clock::now();
    auto const ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    seqan3::debug_stream << "total_hits\t" << g_total_hits << "\n";
    seqan3::debug_stream << "search_ms\t" << ms << "\n";
    return 0;
}
