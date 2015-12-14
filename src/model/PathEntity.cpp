#include "PathEntity.h"

#include <functional>
#include <SFML/System/Vector2.hpp>
#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"

using namespace si::model;

PathEntity::PathEntity(PhysicsProperties physProps, PathFunction path)
	: PhysicsEntity(physProps), path(path)
{ }

sf::Vector2<double> PathEntity::getPosition() const
{
	return path(getLifetime());
}