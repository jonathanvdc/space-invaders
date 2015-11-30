#pragma once

#include <functional>
#include "Common.h"
#include "Entity.h"
#include "ShipEntity.h"
#include "IController.h"

namespace si
{
	namespace controller
	{
		/// Defines a type of controller
		/// that makes an entity (try to) follow
		/// a path function.
		class PathController final : public IController
		{
		public:
			/// Creates a new path controller from the given
			/// target ship, spring constant, and path.
			PathController(const std::shared_ptr<si::model::ShipEntity>& target, 
				double springConstant, const std::function<Vector2d(duration_t)>& path);

			/// Checks if this controller is still "alive".
			/// A live controller will remain in the controller
			/// list, whereas dead controllers will be eliminated.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		private:
			double springConstant;
			std::shared_ptr<si::model::ShipEntity> target;
			std::function<Vector2d(duration_t)> path;
		};
	}
}