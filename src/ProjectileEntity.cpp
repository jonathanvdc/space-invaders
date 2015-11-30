#include "ProjectileEntity.h"

#include <functional>
#include <memory>
#include <vector>
#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"
#include "Game.h"

using namespace si;
using namespace si::model;

ProjectileEntity::ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, double damage,
	ProjectileCreationFunction nestedProjectiles)
	: DriftingEntity(physProps, initialPos), damage(damage), nestedProjectiles(nestedProjectiles)
{ }

ProjectileEntity::ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, double damage)
	: ProjectileEntity(physProps, initialPos, damage, 
		[]() { return std::vector<std::shared_ptr<ProjectileEntity>>(); })
{ }

double ProjectileEntity::getDamage() const
{
	return this->damage;
}

std::vector<std::shared_ptr<ProjectileEntity>> ProjectileEntity::createNestedProjectiles() const
{
	return this->nestedProjectiles();
}