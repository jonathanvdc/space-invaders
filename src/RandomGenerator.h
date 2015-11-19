#pragma once

#include <random>

namespace si
{
	class RandomGenerator
	{
	public:
		RandomGenerator(const RandomGenerator&) = delete;

		template<typename T>
		T nextReal(T min, T max)
		{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(twister);
		}

		template<typename T>
		T nextInt(T min, T max)
		{
			std::uniform_int_distribution<T> dist(min, max);
			return dist(twister);
		}

		static RandomGenerator instance;

	private:
		RandomGenerator()
		{ }

		std::mt19937 twister = std::mt19937(std::random_device()());
	};
}