#include "RenderContext.h"

#include <SFML/Graphics.hpp>

using namespace si;
using namespace si::view;

RenderContext::RenderContext(sf::RenderTarget& target, duration_t timeDelta)
	: target(target), timeDelta(timeDelta)
{ }

sf::RenderTarget& RenderContext::getTarget()
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

duration_t RenderContext::getTimeDelta() const
{
	return this->timeDelta;
}