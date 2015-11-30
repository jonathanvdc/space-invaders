#include "ProjectileCollisionController.h"

#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"
#include "ProjectileEntity.h"
#include "IController.h"
#include "CollisionControllerBase.h"

using namespace si;
using namespace si::controller;

ProjectileCollisionController::ProjectileCollisionController(const std::shared_ptr<si::model::ProjectileEntity>& projectile)
	: projectile(projectile)
{ }

/// Gets the entity this collision controller detects collisions
/// for.
std::shared_ptr<si::model::PhysicsEntity> ProjectileCollisionController::getEntity() const
{
	return this->projectile;
}

/// Handles a collision with another entity.
void ProjectileCollisionController::handleCollision(si::model::Game& game, const std::shared_ptr<si::model::PhysicsEntity>& other)
{
	if (this->isAlive())
	{
		// Simply remove this projectile from the game.
		game.remove(this->projectile);
		
		// Now create nested projectiles from the projectile we
		// just destroyed.
		for (const auto& item : this->projectile->createNestedProjectiles())
		{
			game.add(item);
		}

		this->projectile = nullptr;
	}
}