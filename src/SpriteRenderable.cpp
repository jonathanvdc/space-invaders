#include "SpriteRenderable.h"

#include <SFML/Graphics.hpp>

using namespace si;
using namespace si::view;

SpriteRenderable::SpriteRenderable(
	const std::shared_ptr<sf::Texture>& texture,
	std::function<DoubleRect()> getBounds)
	: texture(texture), getBounds(getBounds)
{ }

void SpriteRenderable::render(RenderContext& context) const
{
	sf::Sprite sprite(*this->texture);
	auto rect = context.transformView(this->getBounds());
	sprite.setPosition((float)rect.left, (float)rect.top);
	auto textureSize = this->texture->getSize();
	sprite.setScale((float)rect.width / textureSize.x, (float)rect.height / textureSize.y);
	context.getTarget().draw(sprite);
}