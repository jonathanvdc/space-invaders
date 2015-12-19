#pragma once

#include "Common.h"
#include "Healthbar.h"
#include "Entity.h"
#include "PhysicsEntity.h"

namespace si
{
	namespace model
	{
		/// Defines an obstacle entity.
		class ObstacleEntity : public PhysicsEntity
		{
		public:
			/// Creates a new obstacle entity from the given physics properties,
			/// initial position and max health.
			ObstacleEntity(PhysicsProperties physProps, Vector2d initialPos, double maxHealth);

            /// Gets this obstacle's health bar.
			Healthbar& getHealth();

			/// Gets this obstacle's health bar.
			const Healthbar& getHealth() const;

			/// Tests if this obstacle entity is still alive.
			bool isAlive() const;

			/// Gets this obstacle entity's position.
			Vector2d getPosition() const final override;

		private:
			Healthbar health;
		};
	}
}
