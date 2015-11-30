#pragma once

#include "Common.h"
#include "IController.h"
#include "ShipEntity.h"

namespace si
{
	namespace controller
	{
		/// A controller class for the player's ship.
		/// Player controllers convert keyboard input
		/// into changes to the game world.
		class PlayerController final : public IController
		{
		public:
			/// Creates a player controller from the given player and
			/// acceleration multiplier.
			PlayerController(const std::shared_ptr<si::model::ShipEntity>& player, double acceleration);

			/// Checks if this player controller is still alive.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		private:
			/// The player entity.
			std::shared_ptr<si::model::ShipEntity> player;
			/// An acceleration multiplier.
			double accelConst;
		};
	}
}