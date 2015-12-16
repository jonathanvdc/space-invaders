#include "SpriteRenderable.h"

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

SpriteRenderable::SpriteRenderable(const std::shared_ptr<sf::Texture>& texture)
	: texture(texture)
{ }

void SpriteRenderable::render(
	RenderContext& context, DoubleRect bounds,
	const Transformation& transform)
{
	sf::Sprite sprite(*this->texture);
	sprite.setPosition(static_cast<float>(bounds.left), static_cast<float>(bounds.top));
	auto textureSize = this->texture->getSize();
	sprite.setScale(static_cast<float>(bounds.width) / textureSize.x, static_cast<float>(bounds.height) / textureSize.y);
	context.getTarget().draw(sprite, transform.toRenderState());
}