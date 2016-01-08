#pragma once

#include <random>

namespace si
{
	/// A singleton class that generates random numbers.
	class RandomGenerator final
	{
	public:
		/// Disallow copying the random number generator: this is a
		/// singleton class which will never go out of scope,
		/// so it can just be passed around by reference.
		RandomGenerator(const RandomGenerator&) = delete;

		/// Conjures up a random floating-point number that is of the given type
		/// and within the given range.
		template<typename T>
		T nextReal(T min, T max)
		{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(twister);
		}

		/// Conjures up a random integer number that is of the given type
		/// and within the given range.
		template<typename T>
		T nextInt(T min, T max)
		{
			std::uniform_int_distribution<T> dist(min, max);
			return dist(twister);
		}

		/// This random number generator's one and only instance.
		static RandomGenerator instance;

	private:
		RandomGenerator()
		{ }

		std::mt19937 twister = std::mt19937(std::random_device()());
	};
}
