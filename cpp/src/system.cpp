#include "system.hpp"

void limit_memory(rlim_t bytes)
{
    struct rlimit rl;
    rl.rlim_cur = bytes; // Soft limit
    rl.rlim_max = bytes; // Hard limit
    setrlimit(RLIMIT_AS, &rl);
}

long peak_rss()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    return usage.ru_maxrss;
}