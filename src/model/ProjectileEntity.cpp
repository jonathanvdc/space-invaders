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

ProjectileEntity::ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc,
	ProjectileCreationFunction nestedProjectiles)
	: DriftingEntity(physProps, initialPos, initialVeloc), nestedProjectiles(nestedProjectiles)
{ }

ProjectileEntity::ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc)
	: ProjectileEntity(physProps, initialPos, initialVeloc,
		[]() { return std::vector<std::shared_ptr<ProjectileEntity>>(); })
{ }

std::vector<std::shared_ptr<ProjectileEntity>> ProjectileEntity::createNestedProjectiles() const
{
	return this->nestedProjectiles();
}