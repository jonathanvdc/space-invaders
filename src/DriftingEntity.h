#pragma once
#include "Common.h"
#include "PhysicsEntity.h"

namespace si
{
	namespace model
	{
		/// Defines a "drifting" entity: an entity whose 
		/// position is defined by its velocity.
		class DriftingEntity : public PhysicsEntity
		{
		public:
			/// Creates a new drifting entity from the given physics properties 
			/// and initial position.
			DriftingEntity(PhysicsProperties physProps, Vector2d initialPos);

			/// Gets this drifting entity's position.
			virtual Vector2d getPosition() const override;

			/// Accelerates the given drifting entity
			/// by adding the given velocity to
			/// the current velocity.
			void accelerate(Vector2d velocity);

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			virtual void updateTime(duration_t delta) override;
		};
	}
}