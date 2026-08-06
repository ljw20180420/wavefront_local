#pragma once

#include <sys/resource.h>

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

// Helper to reliably prevent compiler optimization without performance penalties
template <typename T>
void doNotOptimize(const T &value)
{
    asm volatile("" : : "g"(value) : "memory");
}
