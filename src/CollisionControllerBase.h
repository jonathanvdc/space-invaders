#pragma once

#include <memory>
#include <vector>
#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"
#include "IController.h"

namespace si
{
	namespace controller 
	{
		/// Defines a base class for collision controllers:
		/// controllers that detect and handle collisions.
		class CollisionControllerBase : public IController
		{
		public:
			/// Gets the entity this collision controller detects collisions
			/// for.
			virtual std::shared_ptr<si::model::PhysicsEntity> getEntity() const = 0;

			/// Checks if this controller is still "alive".
			/// A collision controller is said to be
			/// alive as long as the entity it contains
			/// is not null.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		protected:
			/// Handles a collision with another entity.
			virtual void handleCollision(si::model::Game& game, const si::model::Entity_ptr& other) = 0;
		private:
			/// Defines a vector of collisions which were detected in the 
			/// previous frame. Collisions are not handled immediately, because
			/// that could result in objects getting removed from the
			/// game because of a collision before others have a chance
			/// of detecting the collision. For example, a bullet could
			/// bump into a ship, get removed from the board, and the ship
			/// would be none the wiser. 
			/// To avoid this kind of awkward situation, collisions are handled 
			/// the next frame.
			std::vector<si::model::Entity_ptr> collisionTargets;
		};
	}
}