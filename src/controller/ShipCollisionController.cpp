#include "ShipCollisionController.h"

#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/ShipEntity.h"
#include "IController.h"
#include "CollisionControllerBase.h"

using namespace si;
using namespace si::controller;

ShipCollisionController::ShipCollisionController(const std::shared_ptr<si::model::ShipEntity>& ship)
	: ship(ship)
{ }

/// Gets the entity this collision controller detects collisions
/// for.
std::shared_ptr<si::model::PhysicsEntity> ShipCollisionController::getEntity() const
{
	return this->ship;
}

/// Handles a collision with another entity.
void ShipCollisionController::handleCollision(
	si::model::Game& game,
	const std::shared_ptr<si::model::PhysicsEntity>& other)
{
	if (this->isAlive())
	{
		// A projectile's momentum's unit is:
		//
		//     <mass> * <game board dimension> / s
		//
		// so, a projectile that can traverse the entire
		// board in one second, and has a weight of one,
		// has a momentum of one.
		//
		// Since a ship's health is defined
		// in terms of momentum, we can subtract the
		// ship's momentum from the projectile's momentum,
		// and then subtract the length of the resulting
		// momentum vector from the current ship's health.
		//
		// This design makes colliding with a projectile
		// head-on really dangerous. If, on the other hand,
		// the ship is moving in the same direction
		// as the projectile it's colliding with, and they
		// have approximately the same momentum, then
		// it will barely do any damage.

		auto collisionMomentum = this->ship->getMomentum() - other->getMomentum();

		// Decrement the ship's health.
		auto& healthbar = this->ship->getHealth();
		healthbar.setHealth(healthbar.getHealth() - vecLength(collisionMomentum));

		if (!this->ship->isAlive())
		{
			// We may have killed the ship by decrementing
			// its health. I know this kind of sucks for
			// whoever was on board of the ship, but we
			// have to do our job and remove them from
			// the game now. Sorry. :/

			game.remove(this->ship);
			this->ship = nullptr;
		}
	}
}
