#include "GameRenderer.h"

using namespace si;
using namespace si::view;

GameRenderer::GameRenderer(sf::Color backgroundColor)
	: backgroundColor(backgroundColor)
{ }

void GameRenderer::render(RenderContext& target, DoubleRect bounds)
{
	// First, clear the render target.
	target.getTarget().clear(backgroundColor);

	// Then render the game by rendering all sub-objects.
	for (const auto& item : this->items)
	{
		item->render(target, bounds);
	}
}

/// Gets the game's background color.
sf::Color GameRenderer::getBackgroundColor() const
{
	return this->backgroundColor;
}

/// Sets the game's background color to the
/// given value.
void GameRenderer::setBackgroundColor(sf::Color color)
{
	this->backgroundColor = color;
}