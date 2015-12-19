#include "Healthbar.h"

using namespace si;
using namespace si::model;

Healthbar::Healthbar(double maxHealth)
	: maxHealth(maxHealth), health(maxHealth)
{ }

double Healthbar::getHealth() const
{
	return this->health;
}

void Healthbar::setHealth(double value)
{
	this->health = value;
}

double Healthbar::getMaxHealth() const
{
	return this->maxHealth;
}

bool Healthbar::isAlive() const
{
	return this->health > 0.0;
}
