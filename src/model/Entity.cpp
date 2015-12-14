#include "Entity.h"

#include "Common.h"

using namespace si;
using namespace si::model;

void Entity::updateTime(duration_t delta)
{
	elapsed += delta;
}

duration_t Entity::getLifetime() const
{
	return elapsed;
}

