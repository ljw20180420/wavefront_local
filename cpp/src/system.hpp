#pragma once

#include <sys/resource.h>

void limit_memory(rlim_t bytes);

long peak_rss();
