#pragma once

#include "Common.h"
#include "Healthbar.h"
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

			/// Gets this ship's health bar.
			Healthbar& getHealth();

			/// Gets this ship's health bar.
			const Healthbar& getHealth() const;

			/// Tests if this ship entity is still alive.
			bool isAlive() const;

		private:
			Healthbar health;
		};
	}
}
