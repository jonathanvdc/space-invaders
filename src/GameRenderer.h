#pragma once
#include "IRenderable.h"
#include "Container.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable game object.
		class GameRenderer final : public Container<IRenderable>, public virtual IRenderable
		{
		public:
			/// Creates a game renderer with the given background color.
			GameRenderer(sf::Color backgroundColor);

			/// Renders the entire game.
			void render(RenderContext& target, DoubleRect bounds) const final override;

			/// Gets the game's background color.
			sf::Color getBackgroundColor() const;

			/// Sets the game's background color to the
			/// given value.
			void setBackgroundColor(sf::Color color);

		private:
			/// The game's background color.
			sf::Color backgroundColor;
		};
	}
}