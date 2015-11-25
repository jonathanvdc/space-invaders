#include "GameController.h"

#include "Container.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

/// Updates the game model based on the given time delta.
void GameController::update(si::model::Game& game, duration_t timeDelta) const
{
	for (const auto& item : this->items)
	{
		item->update(game, timeDelta);
	}
}