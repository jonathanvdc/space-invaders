#include "TransformedRenderable.h"

#include <functional>
#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

TransformedRenderable::TransformedRenderable(
	const std::shared_ptr<IRenderable>& contents,
	const std::function<std::pair<Transformation, DoubleRect>(DoubleRect)>& getTransformation)
	: contents(contents), getTransformation(getTransformation)
{ }

/// Renders this transformed renderable object.
void TransformedRenderable::render(
	RenderContext& target, DoubleRect bounds,
	const Transformation&)
{
	auto trans = this->getTransformation(bounds);

	this->contents->render(target, trans.second, trans.first);
}

/// Gets this transformed renderable's contents.
std::shared_ptr<IRenderable> TransformedRenderable::getContents() const
{
	return this->contents;
}