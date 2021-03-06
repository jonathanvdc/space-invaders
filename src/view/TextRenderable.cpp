#include "TextRenderable.h"

#include <algorithm>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

TextRenderable::TextRenderable(const std::string& text, const sf::Font& font, sf::Color textColor)
	: text(text), font(font), textColor(textColor)
{ }

std::string TextRenderable::getText() const
{
	return this->text;
}

void TextRenderable::setText(const std::string& text)
{
	this->text = text;
}

sf::Font TextRenderable::getFont() const
{
	return this->font;
}

void TextRenderable::setFont(sf::Font font)
{
	this->font = font;
}

/// Gets the color that this 
/// object renders text in.
sf::Color TextRenderable::getColor() const
{
	return this->textColor;
}

/// Sets this renderable text object's 
/// text color.
void TextRenderable::setColor(sf::Color color)
{
	this->textColor = color;
}

void TextRenderable::render(
	RenderContext& context, DoubleRect bounds,
	const Transformation& transform)
{
	const unsigned int defaultCharSize = 32;

	sf::Text elem;
	elem.setString(this->text);
	elem.setFont(this->font);
	elem.setPosition(static_cast<float>(bounds.left), static_cast<float>(bounds.top));
	elem.setCharacterSize(defaultCharSize);
	elem.setColor(this->textColor);

	auto localBounds = elem.getLocalBounds();

	double widthRatio = bounds.width / localBounds.width;
	double heightRatio = bounds.height / localBounds.height;
	double ratio = std::min(widthRatio, heightRatio);
	unsigned int charSize = static_cast<unsigned int>(defaultCharSize * ratio);

	elem.setCharacterSize(charSize);	

	context.getTarget().draw(elem, transform.toRenderState());
}
