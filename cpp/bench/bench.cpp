#include "bench.hpp"

int main(int argc, char **argv)
{
    Bench bench = Bench(argc, argv);

    if (argc < 3)
    {
        std::cerr << "Error: lib is not provided\n";
        exit(EXIT_FAILURE);
    }
    std::string lib = argv[2];

    std::string method;
    if (lib == "parasail")
    {
        if (argc < 4)
        {
            std::cerr << "Error: method is not provided for parasail\n";
            exit(EXIT_FAILURE);
        }
        method = argv[3];
    }

    std::unique_ptr<BenchBase> bench_base;
    if (lib == "lwfa")
    {
        bench_base = std::make_unique<BenchLwfa>();
    }
    else if (lib == "lbiwfa")
    {
        bench_base = std::make_unique<BenchLbiwfa>();
    }
    else if (lib == "parasail")
    {
        bench_base = std::make_unique<BenchParasail>(method);
    }
    else if (lib == "seqan")
    {
        bench_base = std::make_unique<BenchSeqan>();
    }
    else
    {
        std::cerr << "Error: unknown lib " << lib << '\n';
        exit(EXIT_FAILURE);
    }

    for (const auto &[t, q] : bench)
    {
        bench.tic();
        bench_base->align(t, q);
        bench.toc();
    }
    std::cout << std::fixed << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
