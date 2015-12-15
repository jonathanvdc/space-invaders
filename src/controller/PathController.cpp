#include "PathController.h"

#include <cmath>
#include <functional>
#include "Common.h"
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

PathController::PathController(const std::shared_ptr<si::model::ShipEntity>& target,
	double springConstant, const std::function<Vector2d(duration_t)>& path)
	: target(target), springConstant(springConstant), path(path)
{ }

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool PathController::isAlive() const
{
	return this->target->isAlive();
}

/// Updates the game model based on the given time delta.
void PathController::update(si::model::Game&, duration_t timeDelta) 
{
	// To make an entity trace a path, we will model
	// a critically dampened spring. This kind of spring
	// will move our entity toward the target point in
	// the path, without overshooting and bouncing.

	// Compute the position where we want the entity
	// to end up in the next frame.
	auto targetPos = path(this->target->getLifetime() + timeDelta);

	// Compute the direction in which the entity should travel.
	// Our spring simulation will apply a force in this direction.
	auto direction = targetPos - this->target->getPosition();

	// Calculate the force exerted by the spring.
	auto springForce = this->springConstant * direction;
	// Compute the damping force.
	auto dampingForce = -2 * std::sqrt(this->springConstant) * this->target->getVelocity();

	// The total force to apply is the sum of the above
	// forces.
	auto totalForce = springForce + dampingForce;
	// Now accelerate the entity according to
	// the force we computed.
	this->target->accelerate(totalForce * timeDelta.count());
}
