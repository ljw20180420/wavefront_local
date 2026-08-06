#pragma once

#include <sys/resource.h>

void limit_memory(rlim_t bytes);

long peak_rss();

// Helper to reliably prevent compiler optimization without performance penalties
template <typename T>
void doNotOptimize(const T &value)
{
    asm volatile("" : : "g"(value) : "memory");
}
