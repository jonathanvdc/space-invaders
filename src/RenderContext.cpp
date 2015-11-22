#include "RenderContext.h"

#include <SFML/Graphics.hpp>

using namespace si;
using namespace si::view;

RenderContext::RenderContext(sf::RenderTarget& target)
	: target(target)
{ }

sf::RenderTarget& RenderContext::getTarget() const
{
	return this->target;
}

Vector2d RenderContext::transformView(Vector2d vec) const
{
	Vector2d size(this->target.getSize());
	return{ size.x * vec.x, size.y * vec.y };
}

DoubleRect RenderContext::transformView(DoubleRect rect) const
{
	Vector2d size(this->target.getSize());
	return DoubleRect(size.x * rect.left, size.y * rect.top, size.x * rect.width, size.y * rect.height);
}

double RenderContext::transformView(double scalar) const
{
	Vector2d size(this->target.getSize());
	return std::sqrt(size.x * size.y) * scalar;
}