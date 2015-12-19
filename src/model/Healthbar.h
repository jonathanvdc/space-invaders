#pragma once

#include "Common.h"
#include "Entity.h"
#include "DriftingEntity.h"

namespace si
{
	namespace model
	{
		/// Defines a health bar.
		class Healthbar final
		{
		public:
			/// Creates a health bar from the given maximal health.
			Healthbar(double maxHealth);

			/// Gets this health bar's current health.
			/// Health is defined as the total amount of
			/// momentum from impacts a stationary
			/// entity can survive.
			double getHealth() const;

			/// Sets this health bar's current health to the given value.
			/// Health is defined as the total amount of
			/// momentum from impacts a stationary
			/// entity can survive.
			void setHealth(double value);

			/// Gets this health bar's max health.
			/// Health is defined as the total amount of
			/// momentum from impacts a stationary
			/// entity can survive.
			double getMaxHealth() const;

			/// Tests if the current amount of health is greater
            /// than zero.
			bool isAlive() const;

		private:
			const double maxHealth;
			double health;
		};
	}
}
