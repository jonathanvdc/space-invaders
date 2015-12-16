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
	auto forward = normalizeVec(this->player->getOrientation());
	auto right = normalizeVec(si::Vector2d(-forward.y, forward.x));

	Vector2d accel;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		accel += forward;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		accel -= forward;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		accel -= right;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		accel += right;
	}
	double accelLength = vecLength(accel);
	if (accelLength > 0.0)
	{
		accel *= accelConst / accelLength;
		player->accelerate(accel * timeDelta.count());
	}
}
