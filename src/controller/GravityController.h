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
		/// Defines a type of controller that accelerates drifting entities toward
        /// the controlled entity's current position.
		class GravityController final : public IController
		{
		public:
			/// Creates a gravity controller from the given gravity well
            /// entity, as well as a gravitation constant that determines
            /// how strongly drifting entities are attracted to the target entity.
			/// A negative gravitational constant can be used to create a
			/// "force field" of sorts.
			/// A falloff constant determines the distance
            /// at which entities are affected. A falloff constant of two
            /// will result in inverse-square behavior.
			GravityController(
                const std::shared_ptr<si::model::PhysicsEntity>& target,
				double gravitationalConstant, double falloffConstant = 2.0);

			/// Checks if this controller is still "alive".
			/// A gravity controller is said to be dead once the game no longer
            /// contains its target.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		private:
			std::shared_ptr<si::model::PhysicsEntity> target;
			double gravitationalConstant;
			double falloffConstant;
		};
	}
}
