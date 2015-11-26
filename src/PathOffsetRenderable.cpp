#include "PathOffsetRenderable.h"

#include <functional>
#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"

using namespace si;
using namespace si::view;

PathOffsetRenderable::PathOffsetRenderable(const std::shared_ptr<IRenderable>& contents, const std::function<Vector2d()>& path)
	: contents(contents), getPosition(path)
{ }

/// Renders the relative box renderable's child
/// within a relative box in the given outer bounds.
void PathOffsetRenderable::render(RenderContext& target, DoubleRect bounds)
{
	auto pos = this->getPosition();

	DoubleRect innerBox(
		bounds.left + bounds.width * pos.x,
		bounds.top + bounds.height * pos.y,
		bounds.width,
		bounds.height);

	this->contents->render(target, innerBox);
}

/// Gets this renderable relative box' contents.
std::shared_ptr<IRenderable> PathOffsetRenderable::getContents() const
{
	return this->contents;
}