#include "SpriteRenderable.h"

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

SpriteRenderableBase::SpriteRenderableBase(
	const std::shared_ptr<sf::Texture>& texture)
	: texture(texture)
{ }

void SpriteRenderableBase::render(
	RenderContext& context, DoubleRect bounds,
	const Transformation& transform)
{
	sf::Sprite sprite(*this->texture);
	sprite.setPosition(static_cast<float>(bounds.left), static_cast<float>(bounds.top));
	auto textureSize = this->texture->getSize();
	sprite.setTextureRect(this->getTextureRectangle(context.getTimeDelta()));
	sprite.setScale(static_cast<float>(bounds.width) / textureSize.x, static_cast<float>(bounds.height) / textureSize.y);
	context.getTarget().draw(sprite, transform.toRenderState());
}

/// Gets the texture this sprite renderable uses.
std::shared_ptr<sf::Texture> SpriteRenderableBase::getTexture() const
{
	return this->texture;
}

SpriteRenderable::SpriteRenderable(
	const std::shared_ptr<sf::Texture>& texture)
	: SpriteRenderableBase(texture)
{ }

/// Gets the rectangular area of the texture to render.
sf::IntRect SpriteRenderable::getTextureRectangle(duration_t) const
{
	auto textureSize = this->getTexture()->getSize();
	return{ 0, 0, static_cast<int>(textureSize.x), static_cast<int>(textureSize.y) };
}
