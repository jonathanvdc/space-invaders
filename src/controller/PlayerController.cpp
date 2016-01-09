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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		accel += forward;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		// Only go backward if that doesn't change the player ship's direction.

		auto vel = this->player->getVelocity();

		if (vecDot(vel - forward * timeDelta.count(), vel) >= 0.0)
			accel -= forward;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		accel -= right;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		accel += right;
	}
	double accelLength = vecLength(accel);
	if (accelLength > 0.0)
	{
		accel *= this->accelConst / accelLength;
		player->accelerate(accel * timeDelta.count());
	}
}
