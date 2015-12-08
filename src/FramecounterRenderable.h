#pragma once

#include <memory>
#include "IRenderable.h"
#include "TextRenderable.h"

namespace si
{
	namespace view
	{
		/// Defines a class for renderable frame counter
		/// objects.
		class FramecounterRenderable final : public IRenderable
		{
		public:
			/// Creates a frame counter from the given font and
			/// text color.
			FramecounterRenderable(const sf::Font& font, sf::Color textColor);

			/// Renders this renderable object on the 
			/// given render context, within the given
			/// bounds, which is given in absolute 
			/// coordinates.
			void render(RenderContext& target, DoubleRect bounds) final override;
		private:
			int framecount;
			duration_t elapsed;
			TextRenderable text;
		};
	}
}

