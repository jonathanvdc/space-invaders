#pragma once

#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"

namespace si
{
	namespace model
	{
		/// Defines a ship entity.
		class ShipEntity final : public DriftingEntity
		{
		public:
			/// Creates a new ship entity from the given physics properties,
			/// initial position and max health.
			ShipEntity(PhysicsProperties physProps, Vector2d initialPos, double maxHealth);

			/// Gets this ship's current health.
			double getHealth() const;
			/// Sets this ship's current health to the given value.
			void setHealth(double value);

			/// Gets this ship's max health.
			double getMaxHealth() const;

			/// Tests if this ship entity is still alive.
			bool isAlive() const;

		private:
			double health;
			double maxHealth;
		};
	}
}

