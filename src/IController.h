#pragma once

#include "Common.h"
#include "Game.h"

namespace si
{
	namespace controller
	{
		/// Defines a base class for controllers.
		class IController
		{
		public:
			virtual ~IController() { }

			/// Checks if this controller is still "alive".
			/// A live controller will remain in the controller
			/// list, whereas dead controllers will be eliminated.
			virtual bool isAlive() const = 0;

			/// Updates the game model based on the given time delta.
			/// It is distinct from Entity::updateTime in that this method
			/// should be used to represent changes caused by some outside force,
			/// such as a human player or a computer AI, whereas Entity::updateTime
			/// may be used to propagate the absence of such forces. For example,
			/// an implementation Newton's first law of motion belongs in the model,
			/// because it is intrinsic to the object it models.
			virtual void update(si::model::Game& game, duration_t timeDelta) = 0;
		};
	}
}