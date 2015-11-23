#pragma once

#include "Common.h"
#include "IController.h"
#include "ShipEntity.h"

namespace si
{
	namespace controller
	{
		/// A controller class for the player's ship.
		class PlayerController final : public IController
		{
		public:
			PlayerController(std::shared_ptr<si::model::ShipEntity> player);

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) const final override;
		private:
			std::shared_ptr<si::model::ShipEntity> player;
		};
	}
}