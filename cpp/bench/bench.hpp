#pragma once

#include <sys/resource.h>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

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
            std::exit(1);
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
    std::string length;
    std::string err_num;
    std::string end_up;
    std::string method;
    std::string data_file;
    double total_time;
    std::chrono::_V2::system_clock::time_point start;

    Bench(int argc, char **argv)
    {
        limit_memory(30UL * 1024UL * 1024UL * 1024UL);

        if (argc < 5)
        {
            std::exit(1);
        }
        length = argv[1];
        err_num = argv[2];
        end_up = argv[3];
        method = argv[4];

        data_file = "paper/sim/data/" + length + '_' + err_num + '_' + end_up + ".csv";
        std::cerr << data_file << '\n';

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

    long
    peak_rss()
    {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        return usage.ru_maxrss;
    }

    template <typename T>
    void doNotOptimize(const T &value)
    {
        asm volatile("" : : "g"(value) : "memory");
    }
};