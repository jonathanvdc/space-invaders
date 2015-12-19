#pragma once

#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/ObstacleEntity.h"
#include "IController.h"
#include "CollisionControllerBase.h"

namespace si
{
	namespace controller
	{
		/// Defines a collision handler for obstacles.
		class ObstacleCollisionController final : public CollisionControllerBase
		{
		public:
			ObstacleCollisionController(
                const std::shared_ptr<si::model::ObstacleEntity>& obstacle);

			/// Gets the entity this collision controller detects collisions
			/// for.
			std::shared_ptr<si::model::PhysicsEntity> getEntity() const final override;
		protected:
			/// Handles a collision with another entity.
			void handleCollision(
				si::model::Game& game,
				const std::shared_ptr<si::model::PhysicsEntity>& other) final override;
		private:
			std::shared_ptr<si::model::ObstacleEntity> obstacle;
		};
	}
}
