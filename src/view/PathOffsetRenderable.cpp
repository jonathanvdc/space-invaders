#include "PathOffsetRenderable.h"

#include <functional>
#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

PathOffsetRenderable::PathOffsetRenderable(
	const std::shared_ptr<IRenderable>& contents, 
	const std::function<Vector2d()>& path)
	: contents(contents), getPosition(path)
{ }

/// Renders this renderable object.
void PathOffsetRenderable::render(
	RenderContext& target, DoubleRect bounds,
	const Transformation& transform)
{
	auto pos = this->getPosition();

	DoubleRect innerBox(
		bounds.left + bounds.width * pos.x,
		bounds.top + bounds.height * pos.y,
		bounds.width,
		bounds.height);

	this->contents->render(target, innerBox, transform);
}

/// Gets this path offset renderable's contents.
std::shared_ptr<IRenderable> PathOffsetRenderable::getContents() const
{
	return this->contents;
}