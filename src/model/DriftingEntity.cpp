#include "DriftingEntity.h"

#include "Common.h"
#include "PhysicsEntity.h"

using namespace si;
using namespace si::model;

DriftingEntity::DriftingEntity(PhysicsProperties physProps, Vector2d initialPos)
	: PhysicsEntity(physProps)
{
	this->prevPos = initialPos;
}

DriftingEntity::DriftingEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc)
	: PhysicsEntity(physProps)
{
	this->prevPos = initialPos;
	this->setVelocity(initialVeloc);
}

Vector2d DriftingEntity::getPosition() const
{
	// The 'previous' position also happens to be the
	// current position here, because drifting entities
	// get their position updated when the time is updated.
	return this->prevPos;
}

/// Sets this drifting entity's position.
void DriftingEntity::setPosition(Vector2d pos)
{
	this->prevPos = pos;
}

void DriftingEntity::accelerate(Vector2d velocity)
{
	// Simply add the given vector to the 
	// current velocity.
	this->setVelocity(this->getVelocity() + velocity);
}

void DriftingEntity::updateTime(duration_t delta)
{
	this->Entity::updateTime(delta);

	this->prevPos += delta.count() * this->getVelocity();
}
