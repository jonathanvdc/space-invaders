#include "ObstacleCollisionController.h"

#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/ObstacleEntity.h"
#include "IController.h"
#include "CollisionControllerBase.h"

using namespace si;
using namespace si::controller;

ObstacleCollisionController::ObstacleCollisionController(
    const std::shared_ptr<si::model::ObstacleEntity>& obstacle)
	: obstacle(obstacle)
{ }

/// Gets the entity this collision controller detects collisions
/// for.
std::shared_ptr<si::model::PhysicsEntity> ObstacleCollisionController::getEntity() const
{
	return this->obstacle;
}

/// Handles a collision with another entity.
void ObstacleCollisionController::handleCollision(
	si::model::Game& game,
	const std::shared_ptr<si::model::PhysicsEntity>& other)
{
	if (this->isAlive())
	{
		// Obstacles are damaged when something hits them, just like ships.

		auto collisionMomentum = this->obstacle->getMomentum() - other->getMomentum();

		// Decrement the obstacle's health.
		auto& healthbar = this->obstacle->getHealth();
		healthbar.setHealth(healthbar.getHealth() - vecLength(collisionMomentum));

		if (!this->obstacle->isAlive())
		{
			game.remove(this->obstacle);
			this->obstacle = nullptr;
		}
	}
}
