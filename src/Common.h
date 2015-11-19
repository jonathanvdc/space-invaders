#pragma once
#include <chrono>
#include <SFML/System/Vector2.hpp>

// Defines a number of aliases for types 
// which are used throughout the project.
// A number of useful helpers functions
// are defined as well.

namespace si
{
	typedef std::chrono::duration<double> duration_t;
	typedef sf::Vector2<double> Vector2d;

	/// Computes the square of the length 
	/// of the given vector.
	template<typename T>
	auto vecLengthSqr(sf::Vector2<T> vec)
	{
		return vec.x * vec.x + vec.y * vec.y;
	}

	/// Computes the length of the given vector.
	template<typename T>
	auto vecLength(sf::Vector2<T> vec)
	{
		return std::sqrt(vecLengthSqr(vec));
	}
}