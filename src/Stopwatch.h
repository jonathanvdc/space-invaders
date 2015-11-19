#pragma once
#include <chrono>
#include "Common.h"

namespace si
{
	class Stopwatch
	{
	public:
		Stopwatch(const Stopwatch&) = delete;

		static Stopwatch instance;

		/// Gets the number of seconds since this
		/// clock's delta was last requested.
		duration_t delta();
	private:
		typedef std::chrono::time_point<std::chrono::high_resolution_clock, duration_t> time_point_t;

		Stopwatch();

		// This time_point refers to the previous point in time the clock's time
		// was taken, as a double-precision floating-point number in seconds.
		time_point_t prev;
	};
}

