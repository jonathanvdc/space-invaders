#include "GroupRenderable.h"

#include <vector>
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

using namespace si;
using namespace si::view;

/// Creates a new empty group renderable.
GroupRenderable::GroupRenderable()
	: children()
{ }

/// Creates a group renderable from the given
/// vector of child renderables.
GroupRenderable::GroupRenderable(const std::vector<IRenderable_ptr>& children)
	: children(children)
{ }

/// Creates a group renderable from the given
/// vector of child renderables. Move semantics
/// are used for efficiency.
GroupRenderable::GroupRenderable(std::vector<IRenderable_ptr>&& children)
	: children(children)
{ }

/// Renders this renderable object on the 
/// given render context, within the given
/// bounds, which is given in absolute 
/// coordinates.
void GroupRenderable::render(
	RenderContext& target, DoubleRect bounds,
	const Transformation& transform)
{
	for (const auto& item : this->children)
	{
		item->render(target, bounds, transform);
	}
}