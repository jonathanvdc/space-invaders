#include "RelativeBoxRenderable.h"

#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"

using namespace si;
using namespace si::view;

RelativeBoxRenderable::RelativeBoxRenderable(const std::shared_ptr<IRenderable>& contents, DoubleRect box)
	: contents(contents), box(box)
{ }

/// Renders the relative box renderable's child
/// within a relative box in the given outer bounds.
void RelativeBoxRenderable::render(RenderContext& target, DoubleRect bounds)
{
	DoubleRect innerBox(
		bounds.left + bounds.width * this->box.left,
		bounds.top + bounds.height * this->box.top,
		bounds.width * this->box.width,
		bounds.height * this->box.height);

	this->contents->render(target, innerBox);
}

/// Gets this renderable relative box' contents.
std::shared_ptr<IRenderable> RelativeBoxRenderable::getContents() const
{
	return this->contents;
}

/// Gets the relative box that is used to 
/// render contents in.
DoubleRect RelativeBoxRenderable::getRelativeBox() const
{
	return this->box;
}