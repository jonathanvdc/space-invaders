#pragma once

#include <functional>
#include "Common.h"
#include "model/Entity.h"
#include "model/DriftingEntity.h"
#include "model/PhysicsEntity.h"
#include "IController.h"

namespace si
{
	namespace controller
	{
		/// Defines a type of controller accelerates drifting entities toward
        /// the controlled entity's current position.
		class GravityController final : public IController
		{
		public:
			/// Creates a gravity controller from the given gravity well
            /// entity, as well as a gravitation constant that determines
            /// how much drifting entities are attracted to the target entity.
			GravityController(
                const std::shared_ptr<si::model::PhysicsEntity>& target,
				double gravitationalConstant);

			/// Checks if this controller is still "alive".
			/// A gravity controller is said to be dead once the game no longer
            /// contains its target.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		private:
			std::shared_ptr<si::model::PhysicsEntity> target;
			double gravitationalConstant;
		};
	}
}
