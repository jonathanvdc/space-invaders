#include "PhysicsEntity.h"

#include <algorithm>
#include "Common.h"
#include "Entity.h"

using namespace si;
using namespace si::model;

/// Creates a new physics entity from the given physics
/// properties.
PhysicsEntity::PhysicsEntity(PhysicsProperties props)
	: physProps(props)
{ }

Vector2d PhysicsEntity::getVelocity() const
{
	return this->velocity;
}

Vector2d PhysicsEntity::getMomentum() const
{
	return this->getPhysicsProperties().mass * this->getVelocity();
}

PhysicsProperties PhysicsEntity::getPhysicsProperties() const
{
	return this->physProps;
}

void PhysicsEntity::setVelocity(Vector2d value)
{
	this->velocity = value;
}

bool PhysicsEntity::overlaps(const PhysicsEntity& other) const
{
	double distSquared = vecLengthSqr(this->getPosition() - other.getPosition());
	double totalRadius = this->getPhysicsProperties().radius + other.getPhysicsProperties().radius;
	return distSquared < totalRadius * totalRadius;
}

void PhysicsEntity::updateTime(duration_t delta)
{
	this->Entity::updateTime(delta);

	// Update the physics entity's velocity
	// by performing a simple numerical derivation
	// on the distance traveled.
	auto currentPos = this->getPosition();
	auto posDelta = currentPos - this->prevPos;
	this->setVelocity(posDelta / delta.count());
	// Update the 'previous' position, which
	// is actually the current position now.
	this->prevPos = currentPos;
}