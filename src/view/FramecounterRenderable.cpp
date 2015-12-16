#include "FramecounterRenderable.h"

#include <chrono>
#include <memory>
#include "IRenderable.h"
#include "TextRenderable.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;
using namespace std::chrono_literals;

/// Creates a frame counter from the given font and
/// text color.
FramecounterRenderable::FramecounterRenderable(const sf::Font& font, sf::Color textColor)
	: framecount(0), elapsed(0.0s), text("", font, textColor)
{ }

/// Renders this renderable object on the 
/// given render context, within the given
/// bounds, which is given in absolute 
/// coordinates.
void FramecounterRenderable::render(
	RenderContext& target, DoubleRect bounds,
	const Transformation& transform)
{
	this->elapsed += target.getTimeDelta();
	this->framecount++;

	if (elapsed > 1.0s)
	{
		this->text.setText(std::to_string(framecount / elapsed.count()) + "fps");
		this->framecount = 0;
		this->elapsed -= 1.0s;
	}

	this->text.render(target, bounds, transform);
}