#include "PlayerController.h"

#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include "Common.h"
#include "IController.h"
#include "model/ShipEntity.h"

using namespace si;
using namespace si::controller;

PlayerController::PlayerController(const std::shared_ptr<si::model::ShipEntity>& player, double acceleration)
	: player(player), accelConst(acceleration)
{ }

bool PlayerController::isAlive() const
{
	return this->player->isAlive();
}

/// Updates the game model based on the given time delta.
void PlayerController::update(si::model::Game&, duration_t timeDelta)
{
	Vector2d accel;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		accel += Vector2d(0.0, -1.0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		accel += Vector2d(0.0, 1.0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		accel += Vector2d(-1.0, 0.0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		accel += Vector2d(1.0, 0.0);
	}
	double accelLength = vecLength(accel);
	if (accelLength > 0.0)
	{
		accel *= accelConst / accelLength;
		player->accelerate(accel * timeDelta.count());
	}
}
