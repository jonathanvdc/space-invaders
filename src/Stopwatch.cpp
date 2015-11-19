#include "Stopwatch.h"
#include <chrono>

using namespace si;

Stopwatch::Stopwatch()
{
	(void)delta();
}

Stopwatch Stopwatch::instance{};

duration_t Stopwatch::delta()
{
	time_point_t newTime = std::chrono::high_resolution_clock::now();
	duration_t diff = newTime - prev;
	prev = newTime;
	return diff;
}