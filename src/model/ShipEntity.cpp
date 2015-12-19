#include "ShipEntity.h"

#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"

using namespace si;
using namespace si::model;

ShipEntity::ShipEntity(PhysicsProperties physProps, Vector2d initialPos, double maxHealth)
	: DriftingEntity(physProps, initialPos), health(maxHealth)
{ }

/// Gets this ship's health bar.
Healthbar& ShipEntity::getHealth()
{
    return this->health;
}

/// Gets this ship's health bar.
const Healthbar& ShipEntity::getHealth() const
{
    return this->health;
}

bool ShipEntity::isAlive() const
{
	return this->health.isAlive();
}
