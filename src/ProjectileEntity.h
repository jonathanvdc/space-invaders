#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"
#include "Game.h"

namespace si
{
	namespace model
	{
		/// Defines a type of entity that represents a 
		/// projectile, such as a bullet.
		class ProjectileEntity final : public DriftingEntity
		{
		public:
			typedef std::function<std::vector<std::shared_ptr<ProjectileEntity>>()> ProjectileCreationFunction;

			/// Creates a new projectile entity from the given physics properties,
			/// initial position, damage, and a function that creates a 
			/// sequence of nested projectiles.
			ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, double damage,
				ProjectileCreationFunction createNestedProjectiles);

			/// Creates a new projectile entity from the given physics properties,
			/// initial position and damage. This projectile will not spawn any
			/// additional projectiles.
			ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, double damage);

			/// Gets this amount of damage this projectile does
			/// when it collides with an entity.
			double getDamage() const;

			/// Creates a sequence of nested projectiles for this projectile.
			std::vector<std::shared_ptr<ProjectileEntity>> createNestedProjectiles() const;

		private:
			double damage;
			ProjectileCreationFunction nestedProjectiles;
		};
	}
}

