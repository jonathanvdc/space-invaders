#include "PathEntity.h"

#include <functional>
#include <SFML/System/Vector2.hpp>
#include "Entity.h"

using namespace si::model;

PathEntity::PathEntity(std::function<sf::Vector2<double>(time_delta)> path)
	: elapsed(0), path(path)
{ }

sf::Vector2<double> PathEntity::getPosition() const
{
	return path(elapsed);
}

void PathEntity::updateTime(time_delta delta)
{
	elapsed += delta;
}