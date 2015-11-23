#include "ShipEntity.h"

#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"

using namespace si;
using namespace si::model;

ShipEntity::ShipEntity(PhysicsProperties physProps, Vector2d initialPos, double maxHealth)
	: DriftingEntity(physProps, initialPos), maxHealth(maxHealth), health(maxHealth)
{ }

double ShipEntity::getHealth() const
{
	return this->health;
}

void ShipEntity::setHealth(double value)
{
	this->health = value;
}

double ShipEntity::getMaxHealth() const
{
	return this->maxHealth;
}