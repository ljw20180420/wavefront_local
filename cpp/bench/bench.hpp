#pragma once

#include "../src/lwfa.hpp"
#include "../src/lbiwfa.hpp"
#include "../parasail/parasail.h"
#include "../src/score.hpp"
#include <seqan3/alphabet/all.hpp>
#include <seqan3/alignment/all.hpp>
#include <seqan3/utility/range/all.hpp>

#include <sys/resource.h>
#include <vector>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <tuple>

class BenchIterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::pair<std::string, std::string>;
    using difference_type = std::ptrdiff_t;
    using pointer = const std::pair<std::string, std::string> *;
    using reference = const std::pair<std::string, std::string> &;

    BenchIterator()
    {
        fin_ptr = nullptr;
    }

    BenchIterator(const std::string &data_file)
    {
        fin_ptr = new std::ifstream(data_file);
        if (!fin_ptr->is_open())
        {
            std::cerr << "Error: Could not open file.\n";
            std::exit(EXIT_FAILURE);
        }
        // neglect header
        this->operator++();
        // read first pair of text and query
        this->operator++();
    }

    ~BenchIterator()
    {
        if (fin_ptr)
        {
            fin_ptr->close();
            delete fin_ptr;
        }
    }

    const std::pair<std::string, std::string> &operator*() const { return text_query; }
    const std::pair<std::string, std::string> *operator->() const { return &text_query; }

    BenchIterator &operator++()
    {
        this->operator++(0);
        return *this;
    }

    void operator++(int)
    {
        std::string line;
        if (fin_ptr && std::getline(*fin_ptr, line))
        {
            std::stringstream ss(line);
            std::string text, query;
            std::getline(ss, text, ',');
            std::getline(ss, query);
            text_query = {text, query};
        }
        else
        {
            fin_ptr = nullptr; // Reach the end, detach
        }
    }

    bool operator==(const BenchIterator &other) const
    {
        return fin_ptr == other.fin_ptr;
    }

    bool operator!=(const BenchIterator &other) const
    {
        return !(*this == other);
    }

private:
    std::ifstream *fin_ptr;
    std::pair<std::string, std::string> text_query;
};

class Bench
{
private:
    void limit_memory(rlim_t bytes)
    {
        struct rlimit rl;
        rl.rlim_cur = bytes; // Soft limit
        rl.rlim_max = bytes; // Hard limit
        setrlimit(RLIMIT_AS, &rl);
    }

public:
    std::string data_file;
    double total_time;
    std::chrono::_V2::system_clock::time_point start;

    Bench(int argc, char **argv)
    {
        limit_memory(30UL * 1024UL * 1024UL * 1024UL);

        if (argc < 2)
        {
            std::cerr << "Error: data file is not provided\n";
            exit(EXIT_FAILURE);
        }
        data_file = argv[1];

        total_time = 0.0;
    }

    BenchIterator begin()
    {
        return BenchIterator(data_file);
    }

    BenchIterator end()
    {
        return BenchIterator();
    }

    void tic()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    void toc()
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        total_time += elapsed.count();
    }

    long peak_rss()
    {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        return usage.ru_maxrss;
    }
};

class BenchBase
{
public:
    template <typename T>
    void doNotOptimize(const T &value)
    {
        asm volatile("" : : "g"(value) : "memory");
    }

    virtual void align(const std::string &t, const std::string &q) {}
};

class BenchLwfa : public BenchBase
{
private:
    char c_f, c_r;
    FrontXM FrXM;
    Front FrI, FrD;

public:
    BenchLwfa()
    {
        c_f = 'L';
        c_r = 'L';
    }

    void align(const std::string &t, const std::string &q)
    {
        auto [s_t, d_t, f_t] = lwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
        std::string aln = track<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
        doNotOptimize(aln);

        // std::reverse(aln.begin(), aln.end());
        // int32_t s_t_aln = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
        // if (std::cmp_not_equal(s_t, s_t_aln))
        // {
        //     std::cerr << "inconsistant score\n";
        //     std::exit(EXIT_FAILURE);
        // }
    }
};

class BenchLbiwfa : public BenchBase
{
private:
    char c_f, c_r;
    BiTable<bowtie_wfa.p> BiFr_f, BiFr_r;
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

public:
    BenchLbiwfa()
    {
        c_f = 'L';
        c_r = 'L';
    }

    void align(const std::string &t, const std::string &q)
    {
        aln.resize(t.size() + q.size());
        char *aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln.data());
        aln.resize(aln_t - aln.data());
        int32_t s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
        doNotOptimize(s_t);
    }
};

class BenchParasail : public BenchBase
{
private:
    int32_t l_c, x_c, o_c, e_c;
    parasail_matrix_t *matrix;
    parasail_result_t *result;
    std::string method;

public:
    BenchParasail(std::string method_)
    {
        l_c = bowtie_score.l_c;
        x_c = bowtie_score.x_c;
        o_c = bowtie_score.o_c;
        e_c = bowtie_score.e_c;
        matrix = parasail_matrix_create("ACGT", l_c, -x_c);
        method = method_;
    }

    void align(const std::string &t, const std::string &q)
    {
        result = nullptr;
        if (method == "striped")
        {
            result = parasail_sw_trace_striped_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else if (method == "scan")
        {
            result = parasail_sw_trace_scan_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else if (method == "diag")
        {
            result = parasail_sw_trace_diag_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else
        {
            std::cerr << "Error: unknown method " << method << '\n';
            exit(EXIT_FAILURE);
        }
        if (result == nullptr)
        {
            std::cerr << "Error: parasail_sw_trace_striped_sat failed to allocate memory or initialize.\n";
            exit(EXIT_FAILURE);
        }
        parasail_cigar_t *cigar = parasail_result_get_cigar(result, t.c_str(), t.size(), q.c_str(), q.size(), matrix);
        doNotOptimize(cigar);
    }
};

class BenchSeqan : public BenchBase
{
private:
    int32_t l_c, x_c, o_c, e_c;
    seqan3::configuration<
        seqan3::align_cfg::method_local,
        seqan3::align_cfg::scoring_scheme<seqan3::nucleotide_scoring_scheme<int8_t>>,
        seqan3::align_cfg::gap_cost_affine>
        config;

public:
    BenchSeqan()
    {
        l_c = bowtie_score.l_c;
        x_c = bowtie_score.x_c;
        o_c = bowtie_score.o_c;
        e_c = bowtie_score.e_c;
        config = seqan3::align_cfg::method_local{} |
                 seqan3::align_cfg::scoring_scheme{
                     seqan3::nucleotide_scoring_scheme{
                         seqan3::match_score{l_c},
                         seqan3::mismatch_score{-x_c}}} |
                 seqan3::align_cfg::gap_cost_affine{
                     seqan3::align_cfg::open_score{-o_c},
                     seqan3::align_cfg::extension_score{-e_c}};
    }

    void align(const std::string &t, const std::string &q)
    {
        auto s1 = t | seqan3::views::char_to<seqan3::dna4> | seqan3::ranges::to<seqan3::dna4_vector>();
        auto s2 = q | seqan3::views::char_to<seqan3::dna4> | seqan3::ranges::to<seqan3::dna4_vector>();

        // Invoke the pairwise alignment which returns a lazy range over alignment results.
        auto results = seqan3::align_pairwise(std::tie(s1, s2), config);
        auto &res = *results.begin();
        auto const &[gapped_s1, gapped_s2] = res.alignment();
        // Include this because seqan3 lazily skip the backtrack if the actually alignment is not accessed.
        auto view1 = gapped_s1 | seqan3::views::to_char;
        auto view2 = gapped_s2 | seqan3::views::to_char;
        std::string str1{std::ranges::begin(view1), std::ranges::end(view1)};
        std::string str2{std::ranges::begin(view2), std::ranges::end(view2)};

        doNotOptimize(str1);
        doNotOptimize(str2);
    }
};
