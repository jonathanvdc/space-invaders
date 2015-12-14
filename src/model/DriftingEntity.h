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

			/// Creates a new drifting entity from the given physics properties,
			/// initial position and initial velocity.
			DriftingEntity(PhysicsProperties physProps, Vector2d initialPos, Vector2d initialVeloc);

			/// Gets this drifting entity's position.
			virtual Vector2d getPosition() const override;

			/// Sets this drifting entity's position.
			void setPosition(Vector2d pos);

			/// Accelerates the given drifting entity
			/// by adding the given velocity to
			/// the current velocity.
			void accelerate(Vector2d velocity);

			/// Expose PhysicsEntity::setVelocity here.
			/// This is a far more elegant approach than
			/// using accelerate when trying to set the
			/// velocity.
			using PhysicsEntity::setVelocity;

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			virtual void updateTime(duration_t delta) override;
		};
	}
}