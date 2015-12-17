#pragma once

#include <functional>
#include <memory>
#include <vector>
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
			/// initial position, initial velocity, and a function that creates a
			/// sequence of nested projectiles.
			ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc,
				ProjectileCreationFunction createNestedProjectiles);

			/// Creates a new projectile entity from the given physics properties,
			/// initial position and initial velocity. This projectile will not spawn any
			/// additional projectiles.
			ProjectileEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc);

			/// Creates a sequence of nested projectiles for this projectile.
			std::vector<std::shared_ptr<ProjectileEntity>> createNestedProjectiles() const;

		private:
			ProjectileCreationFunction nestedProjectiles;
		};
	}
}
