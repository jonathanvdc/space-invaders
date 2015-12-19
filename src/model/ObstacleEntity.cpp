#include "ObstacleEntity.h"

#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"

using namespace si;
using namespace si::model;

ObstacleEntity::ObstacleEntity(PhysicsProperties physProps, Vector2d initialPos, double maxHealth)
	: PhysicsEntity(physProps), health(maxHealth)
{
	this->prevPos = initialPos;
}

/// Gets this obstacle's health bar.
Healthbar& ObstacleEntity::getHealth()
{
    return this->health;
}

/// Gets this obstacle's health bar.
const Healthbar& ObstacleEntity::getHealth() const
{
    return this->health;
}

bool ObstacleEntity::isAlive() const
{
	return this->health.isAlive();
}
