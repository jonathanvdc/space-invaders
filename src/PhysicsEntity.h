#pragma once
#include "Common.h"
#include "Entity.h"

namespace si
{
	namespace model
	{
		/// Defines a number of properties that relate to
		/// physics simulation.
		struct PhysicsProperties
		{
			/// Creates a new set of physics properties
			/// from the given information.
			PhysicsProperties(double mass, double radius)
				: mass(mass), radius(radius)
			{ }

			/// The object's mass.
			double mass;
			/// The object's radius.
			double radius;
		};

		/// Defines an entity that has a set of
		/// physical properties, such as its size.
		class PhysicsEntity : public Entity
		{
		public:
			/// Creates a new physics entity from the given physics
			/// properties.
			PhysicsEntity(PhysicsProperties props);

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			virtual void updateTime(duration_t delta) override;

			/// Checks if this physics entity overlaps with the 
			/// given other entity. This is useful for
			/// collision detection.
			bool overlaps(const PhysicsEntity& other) const;

			/// Gets this physics entity's velocity.
			Vector2d getVelocity() const;

			/// Gets this physics entity's momentum,
			/// i.e. the current velocity multiplied by
			/// the entity's mass.
			Vector2d getMomentum() const;

			/// Gets this physics entity's physics properties.
			PhysicsProperties getPhysicsProperties() const;
		private:
			// The physics entity's intrinsic physics properties.
			PhysicsProperties physProps;
			// The physics entity's previous position.
			Vector2d prevPos;
			// The physics entity's velocity.
			Vector2d velocity;
		};
	}
}