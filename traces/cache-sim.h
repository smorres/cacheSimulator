#ifndef CACHESIMULATOR_H
#define CACHESIMULATOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <math.h>

struct info{
	double last_used_time;
	std::vector<long long> cache_line;
};

//All cache_policy simulators will return the cache miss rate in type double
class cache-sim{
	public:
		long long total_ref;
			
		long long direct_mapped(int cache_size, int cache_line_size);
		long long set_associative(int cache_size, int cache_line_size, int associativity);
		long long fully_associative(int cache_size, int cache_line_size, std::string replacement_policy);
		long long set_associative_no_alloc_on_write_miss(int cache_size, int cache_line_size, int associativity);
		long long set_associative_nextline_prefetching (int cache_size, int cache_line_size, int associativity);
		long long prefetch_on_miss (int cache_size, int cache_line_size, int associativity);
};
#endif

