#include "Transformation.h"

#include "Common.h"
#include <SFML/Graphics/Transform.hpp>

using namespace si;
using namespace si::view;

/// Creates an empty transformation.
Transformation::Transformation()
	: matrix()
{ }

/// Creates a transformation from the given
/// 3x3 matrix.
Transformation::Transformation(const sf::Transform& matrix)
	: matrix(matrix)
{ }

/// Modifies this transformation such that the given
/// transformation is applied after this transformation
/// has been completed.
Transformation& Transformation::compose(const Transformation& other)
{
	this->matrix *= other.matrix;
	return *this;
}

/// Creates a 3x3 transformation matrix from this
/// transformation.
sf::Transform Transformation::toMatrix() const
{
	return this->matrix;
}

/// Creates a render state from this transformation.
sf::RenderStates Transformation::toRenderState() const
{
	return sf::RenderStates(this->matrix);
}

/// Transforms the given point.
Vector2d Transformation::transformPoint(Vector2d point) const
{
	return Vector2d(this->matrix.transformPoint(sf::Vector2f(point)));
}

/// The identity transformation.
Transformation Transformation::identity{};

const double Transformation::pi = 3.141592653589793238463;

/// Creates a scaling transformation from the 
/// given scaling vector, and an origin vector.
Transformation Transformation::scale(Vector2d factor, Vector2d origin)
{
	sf::Transform matrix;
	matrix.scale(sf::Vector2f(factor), sf::Vector2f(origin));
	return Transformation(matrix);
}

/// Creates a rotation transformation from the given
/// angle (in radians), and the given origin.
Transformation Transformation::rotate(double angle, Vector2d origin)
{
	sf::Transform matrix;
	matrix.rotate(static_cast<float>(angle * 180.0 / pi), sf::Vector2f(origin));
	return Transformation(matrix);
}

/// Creates a translation transformation from 
/// the given offset.
Transformation Transformation::translate(Vector2d offset)
{
	sf::Transform matrix;
	matrix.translate(sf::Vector2f(offset));
	return Transformation(matrix);
}