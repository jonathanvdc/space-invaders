#pragma once

#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/ShipEntity.h"
#include "IController.h"
#include "CollisionControllerBase.h"

namespace si
{
	namespace controller
	{
		/// Defines a collision handler for ships.
		class ShipCollisionController final : public CollisionControllerBase
		{
		public:
			ShipCollisionController(const std::shared_ptr<si::model::ShipEntity>& ship);

			/// Gets the entity this collision controller detects collisions
			/// for.
			std::shared_ptr<si::model::PhysicsEntity> getEntity() const final override;
		protected:
			/// Handles a collision with another entity.
			void handleCollision(
				si::model::Game& game,
				const std::shared_ptr<si::model::PhysicsEntity>& other) final override;
		private:
			std::shared_ptr<si::model::ShipEntity> ship;
		};

	}
}
