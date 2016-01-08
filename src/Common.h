#pragma once

#include <cmath>
#include <chrono>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

// Defines a number of aliases for types
// which are used throughout the project.
// A number of useful helpers functions
// are defined as well.

namespace si
{
	typedef std::chrono::duration<double> duration_t;
	typedef sf::Vector2<double> Vector2d;
	typedef sf::Rect<double> DoubleRect;

	/// Computes the square of the given vector's
	/// length.
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

	/// Normalizes the given vector.
	template<typename T>
	auto normalizeVec(sf::Vector2<T> vec)
	{
		return vec / vecLength(vec);
	}

	/// Tests if the given shared pointer is of the
	/// given type.
	template<typename T, typename V>
	bool isinstance(const std::shared_ptr<V>& ptr)
	{
		return std::dynamic_pointer_cast<T>(ptr) != nullptr;
	}
}
