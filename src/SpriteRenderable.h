#pragma once
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "IRenderable.h"

namespace si
{
	namespace view 
	{
		/// Defines a renderable sprite object.
		class SpriteRenderable final : public IRenderable
		{
		public:
			/// Creates a renderable sprite from the given texture,
			/// as well as a function that computes the sprite's
			/// current bounding rectangle.
			SpriteRenderable(
				const std::shared_ptr<sf::Texture>& texture, 
				std::function<DoubleRect()> getBounds);

			/// Renders this sprite.
			void render(RenderContext& target) const final override;
		private:
			std::shared_ptr<sf::Texture> texture;
			std::function<DoubleRect()> getBounds;
		};
	}
}